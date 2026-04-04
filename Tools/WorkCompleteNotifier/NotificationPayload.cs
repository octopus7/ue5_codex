using System.Globalization;
using System.IO;

namespace WorkCompleteNotifier;

public sealed class NotificationPayload
{
    private NotificationPayload(string title, DateTimeOffset completedAt, TimeSpan? duration, string workPath)
    {
        Title = string.IsNullOrWhiteSpace(title) ? "작업 완료" : title.Trim();
        CompletedAt = completedAt.ToLocalTime();
        Duration = duration is { } value && value < TimeSpan.Zero ? TimeSpan.Zero : duration;
        WorkPath = string.IsNullOrWhiteSpace(workPath) ? Environment.CurrentDirectory : workPath;
    }

    public string Title { get; }

    public DateTimeOffset CompletedAt { get; }

    public TimeSpan? Duration { get; }

    public string WorkPath { get; }

    public string CompletedAtDisplay => CompletedAt.ToString("yyyy-MM-dd HH:mm:ss", CultureInfo.CurrentCulture);

    public string DurationDisplay => Duration is { } value ? FormatDuration(value) : "미지정";

    public static NotificationPayload FromArguments(string[] args)
    {
        var options = CommandLineOptions.Parse(args);
        var title = options.GetValue("title") ?? "작업 완료";
        var completedAt =
            TryParseDateTimeOffset(options.GetValue("completed-at", "completed", "ended-at")) ??
            DateTimeOffset.Now;
        var startedAt = TryParseDateTimeOffset(options.GetValue("started-at", "started"));
        var duration =
            TryParseDuration(options.GetValue("duration")) ??
            TryParseSeconds(options.GetValue("duration-seconds")) ??
            (startedAt is { } start ? completedAt - start : null);
        var workPath = NormalizePath(
            options.GetValue("path", "work-path") ??
            options.Positionals.FirstOrDefault() ??
            Environment.CurrentDirectory);

        return new NotificationPayload(title, completedAt, duration, workPath);
    }

    private static DateTimeOffset? TryParseDateTimeOffset(string? value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return null;
        }

        if (DateTimeOffset.TryParse(value, CultureInfo.CurrentCulture, DateTimeStyles.AssumeLocal, out var currentCultureValue))
        {
            return currentCultureValue;
        }

        if (DateTimeOffset.TryParse(value, CultureInfo.InvariantCulture, DateTimeStyles.AssumeLocal, out var invariantValue))
        {
            return invariantValue;
        }

        return null;
    }

    private static TimeSpan? TryParseDuration(string? value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return null;
        }

        if (TimeSpan.TryParse(value, CultureInfo.InvariantCulture, out var timeSpanValue) ||
            TimeSpan.TryParse(value, CultureInfo.CurrentCulture, out timeSpanValue))
        {
            return timeSpanValue;
        }

        if (double.TryParse(value, NumberStyles.Float, CultureInfo.InvariantCulture, out var secondsOnly))
        {
            return TimeSpan.FromSeconds(secondsOnly);
        }

        if (value.Length > 1 &&
            double.TryParse(value[..^1], NumberStyles.Float, CultureInfo.InvariantCulture, out var amount))
        {
            return char.ToLowerInvariant(value[^1]) switch
            {
                's' => TimeSpan.FromSeconds(amount),
                'm' => TimeSpan.FromMinutes(amount),
                'h' => TimeSpan.FromHours(amount),
                'd' => TimeSpan.FromDays(amount),
                _ => null,
            };
        }

        return null;
    }

    private static TimeSpan? TryParseSeconds(string? value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return null;
        }

        return double.TryParse(value, NumberStyles.Float, CultureInfo.InvariantCulture, out var seconds)
            ? TimeSpan.FromSeconds(seconds)
            : null;
    }

    private static string NormalizePath(string value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return Environment.CurrentDirectory;
        }

        var expanded = Environment.ExpandEnvironmentVariables(value.Trim());

        try
        {
            return Path.GetFullPath(expanded);
        }
        catch
        {
            return expanded;
        }
    }

    private static string FormatDuration(TimeSpan duration)
    {
        duration = duration.Duration();

        var parts = new List<string>();

        if (duration.Days > 0)
        {
            parts.Add($"{duration.Days}일");
        }

        if (duration.Hours > 0 || parts.Count > 0)
        {
            parts.Add($"{duration.Hours}시간");
        }

        if (duration.Minutes > 0 || parts.Count > 0)
        {
            parts.Add($"{duration.Minutes}분");
        }

        if (duration.Seconds > 0 || parts.Count > 0)
        {
            parts.Add($"{duration.Seconds}초");
        }

        if (parts.Count == 0)
        {
            parts.Add($"{duration.TotalMilliseconds:0}ms");
        }

        return string.Join(" ", parts);
    }

    private sealed class CommandLineOptions
    {
        private readonly Dictionary<string, string?> _values;

        private CommandLineOptions(Dictionary<string, string?> values, List<string> positionals)
        {
            _values = values;
            Positionals = positionals;
        }

        public IReadOnlyList<string> Positionals { get; }

        public static CommandLineOptions Parse(string[] args)
        {
            var values = new Dictionary<string, string?>(StringComparer.OrdinalIgnoreCase);
            var positionals = new List<string>();

            for (var index = 0; index < args.Length; index++)
            {
                var current = args[index];

                if (!current.StartsWith("--", StringComparison.Ordinal))
                {
                    positionals.Add(current);
                    continue;
                }

                var token = current[2..];
                var separatorIndex = token.IndexOf('=');

                if (separatorIndex >= 0)
                {
                    values[token[..separatorIndex]] = token[(separatorIndex + 1)..];
                    continue;
                }

                if (index + 1 < args.Length && !args[index + 1].StartsWith("--", StringComparison.Ordinal))
                {
                    values[token] = args[index + 1];
                    index++;
                    continue;
                }

                values[token] = null;
            }

            return new CommandLineOptions(values, positionals);
        }

        public string? GetValue(params string[] keys)
        {
            foreach (var key in keys)
            {
                if (_values.TryGetValue(key, out var value) && !string.IsNullOrWhiteSpace(value))
                {
                    return value;
                }
            }

            return null;
        }
    }
}
