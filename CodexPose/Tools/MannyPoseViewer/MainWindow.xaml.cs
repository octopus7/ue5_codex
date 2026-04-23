using System.Collections.ObjectModel;
using System.IO;
using System.Numerics;
using System.Text.Json;
using System.Windows;
using System.Windows.Threading;
using MannyPoseViewer.Models;

namespace MannyPoseViewer;

public partial class MainWindow : Window
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
        ReadCommentHandling = JsonCommentHandling.Skip,
        AllowTrailingCommas = true
    };

    private readonly ObservableCollection<PoseDefinition> _filteredPoses = [];
    private readonly DispatcherTimer _reloadTimer;
    private List<PoseDefinition> _allPoses = [];
    private PoseViewerDataset? _dataset;
    private FileSystemWatcher? _watcher;
    private string? _loadedDatasetPath;
    private string? _pendingReloadPath;

    public MainWindow()
    {
        InitializeComponent();

        PoseListBox.ItemsSource = _filteredPoses;
        PreviewControl.EmptyMessage = "Export the dataset from the UE editor or open an existing dataset JSON file.";

        _reloadTimer = new DispatcherTimer
        {
            Interval = TimeSpan.FromMilliseconds(350)
        };
        _reloadTimer.Tick += ReloadTimer_OnTick;

        Loaded += MainWindow_OnLoaded;
        Closed += MainWindow_OnClosed;
    }

    private void MainWindow_OnLoaded(object sender, RoutedEventArgs e)
    {
        var defaultPath = FindDefaultDatasetPath();
        if (defaultPath is not null && File.Exists(defaultPath))
        {
            LoadDataset(defaultPath, "Loaded the default export path. The viewer will auto-reload when UE exports again.");
            return;
        }

        DatasetSummaryText.Text = "No dataset loaded";
        DatasetPathText.Text = defaultPath ?? "Could not resolve the default export path from this launch location.";
        StatusText.Text = "Export the dataset from UE to Saved/MannyPoseToolkit/Exports/manny_pose_viewer_dataset.json, or open a JSON file manually.";
    }

    private void MainWindow_OnClosed(object? sender, EventArgs e)
    {
        DisposeWatcher();
    }

    private void OpenJsonButton_OnClick(object sender, RoutedEventArgs e)
    {
        var dialog = new Microsoft.Win32.OpenFileDialog
        {
            Filter = "JSON Files (*.json)|*.json|All Files (*.*)|*.*",
            Title = "Open Manny Pose Viewer Dataset"
        };

        if (dialog.ShowDialog(this) == true)
        {
            LoadDataset(dialog.FileName, "Loaded dataset from disk. Auto-reload is now watching this file.");
        }
    }

    private void ReloadButton_OnClick(object sender, RoutedEventArgs e)
    {
        if (!string.IsNullOrWhiteSpace(_loadedDatasetPath))
        {
            LoadDataset(_loadedDatasetPath, "Reloaded the current dataset from disk.");
        }
    }

    private void SearchBox_OnTextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
    {
        ApplyPoseFilter();
    }

    private void PoseListBox_OnSelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
    {
        var selectedPose = PoseListBox.SelectedItem as PoseDefinition;
        PreviewControl.Dataset = _dataset;
        PreviewControl.Pose = selectedPose;

        if (selectedPose is null)
        {
            PoseNameText.Text = "No pose selected";
            PoseMetaText.Text = "Category";
            PoseNotesText.Text = "Select a pose from the list to inspect its line preview.";
            PoseSourceText.Text = "-";
            PoseHandsText.Text = "-";
            return;
        }

        PoseNameText.Text = selectedPose.Name;
        PoseMetaText.Text = string.IsNullOrWhiteSpace(selectedPose.Category)
            ? "Uncategorized"
            : selectedPose.Category;
        PoseNotesText.Text = string.IsNullOrWhiteSpace(selectedPose.Notes)
            ? "No notes were exported for this pose."
            : selectedPose.Notes;
        PoseSourceText.Text = string.IsNullOrWhiteSpace(selectedPose.SourceFile)
            ? "-"
            : selectedPose.SourceFile;
        PoseHandsText.Text = $"{FallbackLabel(selectedPose.LeftHandPreset)} / {FallbackLabel(selectedPose.RightHandPreset)}";
    }

    private void ReloadTimer_OnTick(object? sender, EventArgs e)
    {
        _reloadTimer.Stop();

        if (!string.IsNullOrWhiteSpace(_pendingReloadPath) && File.Exists(_pendingReloadPath))
        {
            LoadDataset(_pendingReloadPath, "Detected a fresh UE export and reloaded the dataset.");
        }
    }

    private void LoadDataset(string path, string statusMessage)
    {
        var selectedPoseName = (PoseListBox.SelectedItem as PoseDefinition)?.Name;

        if (!TryReadDataset(path, out var dataset, out var error))
        {
            StatusText.Text = $"Failed to load dataset: {error}";
            return;
        }

        foreach (var pose in dataset.Poses)
        {
            pose.BoneOffsets = new Dictionary<string, BoneOffsetDefinition>(
                pose.BoneOffsets ?? new Dictionary<string, BoneOffsetDefinition>(),
                StringComparer.OrdinalIgnoreCase);
        }

        _dataset = dataset;
        _loadedDatasetPath = Path.GetFullPath(path);
        _allPoses = dataset.Poses.ToList();
        DatasetSummaryText.Text = $"{dataset.Skeleton.Bones.Count} bones / {dataset.Poses.Count} poses / {dataset.Skeleton.Name}";
        DatasetPathText.Text = _loadedDatasetPath;
        StatusText.Text = statusMessage;
        ReloadButton.IsEnabled = true;

        ApplyPoseFilter(selectedPoseName);
        ConfigureWatcher(_loadedDatasetPath);
    }

    private void ApplyPoseFilter(string? preferredPoseName = null)
    {
        var query = SearchBox.Text.Trim();
        var previousSelection = preferredPoseName ?? (PoseListBox.SelectedItem as PoseDefinition)?.Name;

        IEnumerable<PoseDefinition> filtered = _allPoses;
        if (!string.IsNullOrWhiteSpace(query))
        {
            filtered = filtered.Where(p =>
                ContainsIgnoreCase(p.Name, query) ||
                ContainsIgnoreCase(p.Category, query) ||
                ContainsIgnoreCase(p.Notes, query));
        }

        var filteredList = filtered.ToList();
        _filteredPoses.Clear();
        foreach (var pose in filteredList)
        {
            _filteredPoses.Add(pose);
        }

        PoseDefinition? selectedPose = null;
        if (!string.IsNullOrWhiteSpace(previousSelection))
        {
            selectedPose = filteredList.FirstOrDefault(p => string.Equals(p.Name, previousSelection, StringComparison.OrdinalIgnoreCase));
        }

        PoseListBox.SelectedItem = selectedPose ?? filteredList.FirstOrDefault();
    }

    private void ConfigureWatcher(string datasetPath)
    {
        DisposeWatcher();

        var directory = Path.GetDirectoryName(datasetPath);
        var fileName = Path.GetFileName(datasetPath);
        if (string.IsNullOrWhiteSpace(directory) || string.IsNullOrWhiteSpace(fileName) || !Directory.Exists(directory))
        {
            return;
        }

        _watcher = new FileSystemWatcher(directory, fileName)
        {
            NotifyFilter = NotifyFilters.FileName | NotifyFilters.LastWrite | NotifyFilters.Size,
            EnableRaisingEvents = true
        };

        _watcher.Changed += Watcher_OnChanged;
        _watcher.Created += Watcher_OnChanged;
        _watcher.Renamed += Watcher_OnRenamed;
    }

    private void Watcher_OnChanged(object sender, FileSystemEventArgs e)
    {
        Dispatcher.Invoke(() =>
        {
            _pendingReloadPath = e.FullPath;
            _reloadTimer.Stop();
            _reloadTimer.Start();
        });
    }

    private void Watcher_OnRenamed(object sender, RenamedEventArgs e)
    {
        Dispatcher.Invoke(() =>
        {
            _pendingReloadPath = e.FullPath;
            _reloadTimer.Stop();
            _reloadTimer.Start();
        });
    }

    private void DisposeWatcher()
    {
        if (_watcher is null)
        {
            return;
        }

        _watcher.EnableRaisingEvents = false;
        _watcher.Dispose();
        _watcher = null;
    }

    private static bool TryReadDataset(string path, out PoseViewerDataset dataset, out string error)
    {
        dataset = new PoseViewerDataset();
        error = string.Empty;

        if (!TryReadAllText(path, out var jsonText, out error))
        {
            return false;
        }

        try
        {
            using var document = JsonDocument.Parse(jsonText);
            var root = document.RootElement;

            if (IsLegacyBundle(root))
            {
                var bundle = JsonSerializer.Deserialize<LegacyPoseBundle>(jsonText, JsonOptions) ?? new LegacyPoseBundle();
                dataset = ConvertLegacyBundle(bundle, path);
            }
            else if (IsLegacyRawPose(root))
            {
                var rawPose = JsonSerializer.Deserialize<LegacyRawPose>(jsonText, JsonOptions) ?? new LegacyRawPose();
                dataset = ConvertSingleLegacyPose(rawPose, path);
            }
            else
            {
                dataset = JsonSerializer.Deserialize<PoseViewerDataset>(jsonText, JsonOptions) ?? new PoseViewerDataset();
            }
        }
        catch (Exception ex)
        {
            error = ex.Message;
            return false;
        }

        if (dataset.Skeleton.Bones.Count == 0)
        {
            dataset.Skeleton = BuiltInMannySkeleton.Create(Path.GetDirectoryName(path) ?? string.Empty, DateTime.UtcNow.ToString("O"));
        }

        return true;
    }

    private static bool TryReadAllText(string path, out string content, out string error)
    {
        content = string.Empty;
        error = string.Empty;

        for (var attempt = 0; attempt < 6; attempt++)
        {
            try
            {
                content = File.ReadAllText(path);
                return true;
            }
            catch (IOException ex) when (attempt < 5)
            {
                error = ex.Message;
                Thread.Sleep(120);
            }
            catch (UnauthorizedAccessException ex) when (attempt < 5)
            {
                error = ex.Message;
                Thread.Sleep(120);
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        return false;
    }

    private static string? FindDefaultDatasetPath()
    {
        var current = new DirectoryInfo(AppContext.BaseDirectory);
        while (current is not null)
        {
            var candidateProject = Path.Combine(current.FullName, "CodexPose.uproject");
            if (File.Exists(candidateProject))
            {
                return Path.Combine(current.FullName, "Saved", "MannyPoseToolkit", "Exports", "manny_pose_viewer_dataset.json");
            }

            current = current.Parent;
        }

        return null;
    }

    private static bool ContainsIgnoreCase(string? value, string query)
    {
        return !string.IsNullOrWhiteSpace(value) &&
               value.Contains(query, StringComparison.OrdinalIgnoreCase);
    }

    private static string FallbackLabel(string? value)
    {
        return string.IsNullOrWhiteSpace(value) ? "none" : value;
    }

    private static bool IsLegacyBundle(JsonElement root)
    {
        return root.TryGetProperty("bundleType", out var bundleType) &&
               bundleType.ValueKind == JsonValueKind.String &&
               string.Equals(bundleType.GetString(), "MannyBodyPoseBundle", StringComparison.OrdinalIgnoreCase);
    }

    private static bool IsLegacyRawPose(JsonElement root)
    {
        return root.TryGetProperty("fkBones", out var fkBones) &&
               fkBones.ValueKind == JsonValueKind.Object;
    }

    private static PoseViewerDataset ConvertLegacyBundle(LegacyPoseBundle bundle, string sourcePath)
    {
        var sourceDirectory = string.IsNullOrWhiteSpace(bundle.SourceDirectory)
            ? Path.GetDirectoryName(sourcePath) ?? string.Empty
            : bundle.SourceDirectory;

        return new PoseViewerDataset
        {
            Version = string.IsNullOrWhiteSpace(bundle.Version) ? "1.0" : bundle.Version,
            DatasetType = "MannyPoseViewerDataset",
            Skeleton = BuiltInMannySkeleton.Create(sourceDirectory, bundle.GeneratedAtUtc),
            Poses = bundle.Poses
                .Select(entry => ConvertLegacyPose(
                    entry.Pose,
                    entry.FileName,
                    string.IsNullOrWhiteSpace(entry.Category) ? entry.Pose.Category : entry.Category,
                    string.IsNullOrWhiteSpace(entry.Notes) ? entry.Pose.Notes : entry.Notes))
                .ToList()
        };
    }

    private static PoseViewerDataset ConvertSingleLegacyPose(LegacyRawPose rawPose, string sourcePath)
    {
        return new PoseViewerDataset
        {
            Version = string.IsNullOrWhiteSpace(rawPose.Version) ? "1.0" : rawPose.Version,
            DatasetType = "MannyPoseViewerDataset",
            Skeleton = BuiltInMannySkeleton.Create(Path.GetDirectoryName(sourcePath) ?? string.Empty, DateTime.UtcNow.ToString("O")),
            Poses = [ConvertLegacyPose(rawPose, Path.GetFileName(sourcePath), rawPose.Category, rawPose.Notes)]
        };
    }

    private static PoseDefinition ConvertLegacyPose(
        LegacyRawPose rawPose,
        string sourceFile,
        string? categoryOverride,
        string? notesOverride)
    {
        var pose = new PoseDefinition
        {
            Name = rawPose.Name,
            Category = categoryOverride ?? string.Empty,
            Notes = notesOverride ?? string.Empty,
            SourceFile = sourceFile,
            LeftHandPreset = rawPose.LeftHandPreset,
            RightHandPreset = rawPose.RightHandPreset,
            BoneOffsets = new Dictionary<string, BoneOffsetDefinition>(StringComparer.OrdinalIgnoreCase)
        };

        foreach (var pair in rawPose.FkBones)
        {
            pose.BoneOffsets[pair.Key] = new BoneOffsetDefinition
            {
                Location = pair.Value.Location ?? [],
                RotationQuaternion = RotatorDegreesToQuaternion(pair.Value.Rotation ?? [])
            };
        }

        return pose;
    }

    private static float[] RotatorDegreesToQuaternion(float[] rotatorDegrees)
    {
        var pitch = rotatorDegrees.Length > 0 ? DegreesToRadians(rotatorDegrees[0]) * 0.5f : 0.0f;
        var yaw = rotatorDegrees.Length > 1 ? DegreesToRadians(rotatorDegrees[1]) * 0.5f : 0.0f;
        var roll = rotatorDegrees.Length > 2 ? DegreesToRadians(rotatorDegrees[2]) * 0.5f : 0.0f;

        var sp = MathF.Sin(pitch);
        var cp = MathF.Cos(pitch);
        var sy = MathF.Sin(yaw);
        var cy = MathF.Cos(yaw);
        var sr = MathF.Sin(roll);
        var cr = MathF.Cos(roll);

        var quaternion = Quaternion.Normalize(new Quaternion(
            (cr * sp * sy) - (sr * cp * cy),
            (-cr * sp * cy) - (sr * cp * sy),
            (cr * cp * sy) - (sr * sp * cy),
            (cr * cp * cy) + (sr * sp * sy)));

        return [quaternion.X, quaternion.Y, quaternion.Z, quaternion.W];
    }

    private static float DegreesToRadians(float degrees)
    {
        return degrees * (MathF.PI / 180.0f);
    }
}
