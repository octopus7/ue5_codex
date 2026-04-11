const DEFAULT_DATA_PATH = "./data/monitor-data.current.json";
const DEFAULT_POLL_SECONDS = 5;

const state = {
  dataPath: null,
  data: null,
  lastRefreshAt: null,
  pollTimerId: null,
  renderTimerId: null,
  loadError: null,
};

const elements = {
  dataPath: document.querySelector("#data-path"),
  metaSummary: document.querySelector("#meta-summary"),
  generatedAt: document.querySelector("#generated-at"),
  statsGrid: document.querySelector("#stats-grid"),
  signalList: document.querySelector("#signal-list"),
  boardBody: document.querySelector("#board-body"),
  lockList: document.querySelector("#lock-list"),
  timelineList: document.querySelector("#timeline-list"),
  refreshButton: document.querySelector("#refresh-button"),
  autoRefreshToggle: document.querySelector("#auto-refresh-toggle"),
  refreshStatus: document.querySelector("#refresh-status"),
  lastRefresh: document.querySelector("#last-refresh"),
};

function escapeHtml(value) {
  return String(value ?? "")
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#39;");
}

function formatTimestamp(value, timezone = "Asia/Seoul") {
  if (!value) {
    return "-";
  }

  const date = new Date(value);
  if (Number.isNaN(date.getTime())) {
    return value;
  }

  return new Intl.DateTimeFormat("ko-KR", {
    dateStyle: "medium",
    timeStyle: "medium",
    timeZone: timezone,
  }).format(date);
}

function formatDuration(milliseconds) {
  if (!Number.isFinite(milliseconds) || milliseconds < 0) {
    return "-";
  }

  const totalSeconds = Math.floor(milliseconds / 1000);
  const days = Math.floor(totalSeconds / 86400);
  const hours = Math.floor((totalSeconds % 86400) / 3600);
  const minutes = Math.floor((totalSeconds % 3600) / 60);
  const seconds = totalSeconds % 60;

  const segments = [];
  if (days > 0) {
    segments.push(`${days}d`);
  }
  if (hours > 0 || days > 0) {
    segments.push(`${hours}h`);
  }
  if (minutes > 0 || hours > 0 || days > 0) {
    segments.push(`${minutes}m`);
  }
  segments.push(`${seconds}s`);
  return segments.join(" ");
}

function getDataPath() {
  const params = new URLSearchParams(window.location.search);
  return params.get("data") || DEFAULT_DATA_PATH;
}

function getRequestUrl(dataPath) {
  const url = new URL(dataPath, window.location.href);
  url.searchParams.set("_ts", Date.now().toString());
  return url.toString();
}

function getElapsedAnchor(item) {
  if (item.state === "진행중") {
    return item.started_at || item.updated_at;
  }
  if (item.state === "대기") {
    return item.waiting_since || item.updated_at;
  }
  if (item.state === "차단") {
    return item.blocked_since || item.updated_at;
  }
  if (item.state === "검증중") {
    return item.verification_started_at || item.updated_at;
  }
  return null;
}

function getElapsedLabel(item) {
  const anchor = getElapsedAnchor(item);
  if (!anchor) {
    return "-";
  }

  const anchorDate = new Date(anchor);
  if (Number.isNaN(anchorDate.getTime())) {
    return "-";
  }

  const prefixMap = {
    "진행중": "진행",
    "대기": "대기",
    "차단": "차단",
    "검증중": "검증",
  };

  return `${prefixMap[item.state] || "경과"} ${formatDuration(Date.now() - anchorDate.getTime())}`;
}

function getLockElapsedLabel(lock) {
  if (!lock.claimed_at || lock.state !== "점유중") {
    return "점유 없음";
  }

  const claimedAt = new Date(lock.claimed_at);
  if (Number.isNaN(claimedAt.getTime())) {
    return "점유 시간 계산 불가";
  }

  return `점유 ${formatDuration(Date.now() - claimedAt.getTime())}`;
}

function summarizeBoard(board) {
  const summary = {
    total: board.length,
    completed: 0,
    active: 0,
    waiting: 0,
    blocked: 0,
    verifying: 0,
  };

  for (const item of board) {
    if (item.state === "완료") {
      summary.completed += 1;
    }
    if (item.state === "진행중") {
      summary.active += 1;
    }
    if (item.state === "대기") {
      summary.waiting += 1;
    }
    if (item.state === "차단") {
      summary.blocked += 1;
    }
    if (item.state === "검증중") {
      summary.verifying += 1;
    }
  }

  summary.completionRatio = summary.total === 0
    ? 0
    : Math.round((summary.completed / summary.total) * 100);

  return summary;
}

function findLongestItemByState(board, targetState) {
  let winner = null;
  let maxElapsed = -1;

  for (const item of board) {
    if (item.state !== targetState) {
      continue;
    }

    const anchor = getElapsedAnchor(item);
    const timestamp = anchor ? new Date(anchor).getTime() : Number.NaN;
    if (!Number.isFinite(timestamp)) {
      continue;
    }

    const elapsed = Date.now() - timestamp;
    if (elapsed > maxElapsed) {
      maxElapsed = elapsed;
      winner = item;
    }
  }

  return winner;
}

function renderSummary(data) {
  const summary = summarizeBoard(data.board);
  const locksInUse = data.locks.filter((lock) => lock.state === "점유중").length;
  const longestWaiting = findLongestItemByState(data.board, "대기");
  const longestBlocked = findLongestItemByState(data.board, "차단");

  const cards = [
    {
      label: "완료율",
      value: `${summary.completionRatio}%`,
      note: `${summary.completed} / ${summary.total} 작업이 batch scope 기준 완료 상태입니다.`,
    },
    {
      label: "활성 작업",
      value: `${summary.active + summary.verifying}`,
      note: `진행중 ${summary.active}, 검증중 ${summary.verifying}개입니다.`,
    },
    {
      label: "대기 트랙",
      value: `${summary.waiting}`,
      note: longestWaiting
        ? `${longestWaiting.id}가 ${getElapsedLabel(longestWaiting)} 상태입니다.`
        : "대기 상태인 작업이 없습니다.",
    },
    {
      label: "차단 트랙",
      value: `${summary.blocked}`,
      note: longestBlocked
        ? `${longestBlocked.id}가 ${getElapsedLabel(longestBlocked)} 상태입니다.`
        : "차단 상태인 작업이 없습니다.",
    },
    {
      label: "리소스 잠금",
      value: `${locksInUse}`,
      note: `${data.locks.length}개 감시 대상 중 ${locksInUse}개가 현재 점유중입니다.`,
    },
  ];

  elements.statsGrid.innerHTML = cards.map((card) => `
    <article class="stat-card">
      <p class="stat-label">${escapeHtml(card.label)}</p>
      <p class="stat-value">${escapeHtml(card.value)}</p>
      <p class="stat-subtext">${escapeHtml(card.note)}</p>
    </article>
  `).join("");
}

function renderSignals(data) {
  const summary = summarizeBoard(data.board);
  const activeLocks = data.locks.filter((lock) => lock.state === "점유중");
  const waitingItems = data.board.filter((item) => item.state === "대기");
  const blockedItems = data.board.filter((item) => item.state === "차단");
  const topTimeline = [...data.timeline]
    .sort((a, b) => new Date(b.timestamp).getTime() - new Date(a.timestamp).getTime())
    .slice(0, 3);

  const signals = [
    {
      title: "활성 병렬 구간",
      copy: summary.active > 1
        ? `현재 ${summary.active}개 트랙이 동시에 진행중입니다. 공용 수정 지점 충돌 여부를 계속 확인해야 합니다.`
        : summary.active === 1
          ? "현재 1개 트랙이 진행중입니다. 다음 병렬 진입 후보를 검토할 시점입니다."
          : "진행중 트랙이 없습니다. 대기 또는 검증 상태를 우선 확인해야 합니다.",
    },
    {
      title: "선행 조건 대기",
      copy: waitingItems.length > 0
        ? waitingItems.map((item) => `${item.id}: ${item.dependency || "대기 조건 확인 필요"}`).join(" / ")
        : "현재 선행 조건 대기중인 작업이 없습니다.",
    },
    {
      title: "차단 또는 위험",
      copy: blockedItems.length > 0
        ? blockedItems.map((item) => `${item.id}: ${item.note || "차단 사유 확인 필요"}`).join(" / ")
        : activeLocks.length > 0
          ? `잠금 점유중 리소스: ${activeLocks.map((lock) => lock.resource).join(", ")}`
          : "즉시 보이는 차단 또는 잠금 충돌 신호는 없습니다.",
    },
    {
      title: "최근 이벤트 포커스",
      copy: topTimeline.length > 0
        ? topTimeline.map((entry) => `${entry.track} ${entry.event}`).join(" / ")
        : "표시할 최근 이벤트가 없습니다.",
    },
  ];

  elements.signalList.innerHTML = signals.map((item) => `
    <article class="signal-card">
      <p class="signal-title">${escapeHtml(item.title)}</p>
      <p class="signal-copy">${escapeHtml(item.copy)}</p>
    </article>
  `).join("");
}

function renderBoard(data) {
  const orderedBoard = [...data.board].sort((left, right) => {
    if (left.priority !== right.priority) {
      return left.priority - right.priority;
    }

    return String(left.id).localeCompare(String(right.id), "ko");
  });

  elements.boardBody.innerHTML = orderedBoard.map((item) => {
    const progressValue = Number.isFinite(item.progress) ? item.progress : 0;

    return `
      <tr>
        <td>
          <div class="id-stack">
            <span class="item-kind">${escapeHtml(item.kind)}</span>
            <strong>${escapeHtml(item.id)}</strong>
          </div>
        </td>
        <td>
          <div class="work-stack">
            <span class="item-title">${escapeHtml(item.title)}</span>
            <span class="item-subline">${escapeHtml(item.scope || "-")}</span>
          </div>
        </td>
        <td>${escapeHtml(item.owner || "-")}</td>
        <td>${escapeHtml(item.dependency || "없음")}</td>
        <td><span class="status-pill" data-state="${escapeHtml(item.state)}">${escapeHtml(item.state)}</span></td>
        <td>
          <div class="progress-shell">
            <div class="progress-bar" style="width: ${Math.max(0, Math.min(100, progressValue))}%"></div>
          </div>
          <div class="progress-label">${escapeHtml(`${progressValue}%`)}</div>
        </td>
        <td><span class="elapsed-label">${escapeHtml(getElapsedLabel(item))}</span></td>
        <td>${escapeHtml(formatTimestamp(item.updated_at, data.meta.timezone))}</td>
        <td>${escapeHtml(item.note || "-")}</td>
      </tr>
    `;
  }).join("");
}

function renderLocks(data) {
  if (data.locks.length === 0) {
    elements.lockList.innerHTML = '<div class="empty-state">표시할 잠금 리소스가 없습니다.</div>';
    return;
  }

  elements.lockList.innerHTML = data.locks.map((lock) => `
    <article class="lock-card">
      <p class="lock-state">${escapeHtml(lock.owner || "-")}</p>
      <div class="timeline-title-row">
        <p class="lock-title">${escapeHtml(lock.resource)}</p>
        <span class="lock-pill" data-state="${escapeHtml(lock.state)}">${escapeHtml(lock.state)}</span>
      </div>
      <p class="lock-meta">
        관련 작업: ${escapeHtml((lock.related_tracks || []).join(", ") || "-")}<br>
        마지막 갱신: ${escapeHtml(formatTimestamp(lock.updated_at, data.meta.timezone))}<br>
        ${escapeHtml(getLockElapsedLabel(lock))}
      </p>
      <p class="lock-note">${escapeHtml(lock.note || "-")}</p>
    </article>
  `).join("");
}

function renderTimeline(data) {
  const orderedTimeline = [...data.timeline].sort(
    (left, right) => new Date(right.timestamp).getTime() - new Date(left.timestamp).getTime(),
  );

  if (orderedTimeline.length === 0) {
    elements.timelineList.innerHTML = '<div class="empty-state">표시할 타임라인 이벤트가 없습니다.</div>';
    return;
  }

  elements.timelineList.innerHTML = orderedTimeline.map((entry) => `
    <article class="timeline-card">
      <div>
        <p class="timeline-track">${escapeHtml(entry.reporter || "-")}</p>
        <p class="timestamp">${escapeHtml(formatTimestamp(entry.timestamp, data.meta.timezone))}</p>
      </div>
      <div>
        <p class="timeline-track">${escapeHtml(entry.kind || "-")}</p>
        <div class="timeline-title-row">
          <p class="timeline-title">${escapeHtml(entry.track || "-")}</p>
          <span class="status-pill" data-state="${escapeHtml(entry.event_state || "진행중")}">${escapeHtml(entry.event)}</span>
        </div>
      </div>
      <div>
        <p class="timeline-copy">${escapeHtml(entry.detail || "-")}</p>
        <p class="timeline-meta">후속 조치: ${escapeHtml(entry.next_action || "-")}</p>
      </div>
    </article>
  `).join("");
}

function renderHeader(data) {
  elements.dataPath.textContent = state.dataPath;
  elements.metaSummary.textContent = data.meta.description || "배치 모니터 샘플";
  document.title = data.meta.title || "Batch Monitor Sample";
  elements.generatedAt.textContent = `데이터 생성 시각: ${formatTimestamp(data.meta.generated_at, data.meta.timezone)}`;
  elements.lastRefresh.textContent = state.lastRefreshAt
    ? `최근 읽기: ${formatTimestamp(state.lastRefreshAt.toISOString(), data.meta.timezone)}`
    : "최근 읽기 없음";

  if (state.loadError) {
    elements.refreshStatus.innerHTML = `<span class="error-banner">${escapeHtml(state.loadError)}</span>`;
    return;
  }

  const pollSeconds = data.meta.refresh_hint_seconds || DEFAULT_POLL_SECONDS;
  elements.refreshStatus.textContent = elements.autoRefreshToggle.checked
    ? `${pollSeconds}초 주기 폴링`
    : "자동 폴링 중지";
}

function renderAll() {
  if (!state.data) {
    elements.dataPath.textContent = state.dataPath || DEFAULT_DATA_PATH;
    elements.metaSummary.textContent = state.loadError || "샘플 데이터를 불러오는 중입니다.";
    elements.generatedAt.textContent = "-";
    elements.lastRefresh.textContent = "최근 읽기 없음";
    elements.refreshStatus.innerHTML = state.loadError
      ? `<span class="error-banner">${escapeHtml(state.loadError)}</span>`
      : "폴링 대기중";
    return;
  }

  renderHeader(state.data);
  renderSummary(state.data);
  renderSignals(state.data);
  renderBoard(state.data);
  renderLocks(state.data);
  renderTimeline(state.data);
}

function stopPolling() {
  if (state.pollTimerId !== null) {
    window.clearInterval(state.pollTimerId);
    state.pollTimerId = null;
  }
}

function startPolling() {
  stopPolling();

  if (!elements.autoRefreshToggle.checked) {
    renderAll();
    return;
  }

  const seconds = state.data?.meta?.refresh_hint_seconds || DEFAULT_POLL_SECONDS;
  state.pollTimerId = window.setInterval(() => {
    void loadData();
  }, seconds * 1000);
  renderAll();
}

async function loadData() {
  try {
    const response = await fetch(getRequestUrl(state.dataPath), { cache: "no-store" });
    if (!response.ok) {
      throw new Error(`데이터 파일을 읽지 못했습니다 (${response.status})`);
    }

    state.data = await response.json();
    state.lastRefreshAt = new Date();
    state.loadError = null;
    renderAll();
    startPolling();
  } catch (error) {
    state.loadError = error instanceof Error ? error.message : "알 수 없는 로드 오류";
    renderAll();
  }
}

function bindEvents() {
  elements.refreshButton.addEventListener("click", () => {
    void loadData();
  });

  elements.autoRefreshToggle.addEventListener("change", () => {
    startPolling();
  });
}

function startRenderClock() {
  if (state.renderTimerId !== null) {
    window.clearInterval(state.renderTimerId);
  }

  state.renderTimerId = window.setInterval(() => {
    renderAll();
  }, 1000);
}

async function bootstrap() {
  state.dataPath = getDataPath();
  bindEvents();
  startRenderClock();
  await loadData();
}

void bootstrap();
