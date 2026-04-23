using System.Globalization;
using System.Numerics;
using System.Windows;
using System.Windows.Media;
using MannyPoseViewer.Models;

namespace MannyPoseViewer.Rendering;

public sealed class PosePreviewControl : FrameworkElement
{
    private static readonly Brush PanelBrush = new LinearGradientBrush(
        Color.FromRgb(0x1A, 0x20, 0x24),
        Color.FromRgb(0x12, 0x17, 0x1B),
        new Point(0.0, 0.0),
        new Point(0.0, 1.0));

    private static readonly Pen FloorPen = new(
        new SolidColorBrush(Color.FromArgb(140, 0x8E, 0x9A, 0x9B)),
        1.0);

    private static readonly Brush MessageBrush = new SolidColorBrush(Color.FromRgb(0xD7, 0xDB, 0xE2));

    private PoseViewerDataset? _dataset;
    private PoseDefinition? _pose;
    private string _emptyMessage = "Open an exported Manny pose dataset JSON file to begin.";

    public PoseViewerDataset? Dataset
    {
        get => _dataset;
        set
        {
            _dataset = value;
            InvalidateVisual();
        }
    }

    public PoseDefinition? Pose
    {
        get => _pose;
        set
        {
            _pose = value;
            InvalidateVisual();
        }
    }

    public string EmptyMessage
    {
        get => _emptyMessage;
        set
        {
            _emptyMessage = value;
            InvalidateVisual();
        }
    }

    protected override void OnRender(DrawingContext drawingContext)
    {
        base.OnRender(drawingContext);

        var bounds = new Rect(0.0, 0.0, ActualWidth, ActualHeight);
        drawingContext.DrawRectangle(PanelBrush, null, bounds);

        if (bounds.Width < 1.0 || bounds.Height < 1.0)
        {
            return;
        }

        var floorY = bounds.Height - 18.0;
        drawingContext.DrawLine(FloorPen, new Point(18.0, floorY), new Point(bounds.Width - 18.0, floorY));

        if (_dataset is null || _pose is null)
        {
            DrawMessage(drawingContext, bounds, _emptyMessage);
            return;
        }

        var segments = PosePreviewMath.BuildSegments(_dataset, _pose);
        if (segments.Count == 0)
        {
            DrawMessage(drawingContext, bounds, "The selected pose does not produce any previewable line segments.");
            return;
        }

        var viewForward = PosePreviewMath.ReadVector3(_dataset.Skeleton.PreviewViewForward, new Vector3(-1.0f, -1.3f, -0.45f));
        var viewUp = PosePreviewMath.ReadVector3(_dataset.Skeleton.PreviewUp, Vector3.UnitZ);

        var projectedSegments = new List<(Point Start, Point End, Brush Brush, double Thickness)>(segments.Count);
        var minX = double.MaxValue;
        var minY = double.MaxValue;
        var maxX = double.MinValue;
        var maxY = double.MinValue;

        foreach (var segment in segments)
        {
            var projectedStart = PosePreviewMath.ProjectPoint(segment.Start, viewForward, viewUp);
            var projectedEnd = PosePreviewMath.ProjectPoint(segment.End, viewForward, viewUp);

            minX = Math.Min(minX, Math.Min(projectedStart.X, projectedEnd.X));
            minY = Math.Min(minY, Math.Min(projectedStart.Y, projectedEnd.Y));
            maxX = Math.Max(maxX, Math.Max(projectedStart.X, projectedEnd.X));
            maxY = Math.Max(maxY, Math.Max(projectedStart.Y, projectedEnd.Y));

            projectedSegments.Add((
                new Point(projectedStart.X, projectedStart.Y),
                new Point(projectedEnd.X, projectedEnd.Y),
                new SolidColorBrush(segment.Color),
                segment.Thickness));
        }

        var padding = new Thickness(22.0, 22.0, 22.0, 24.0);
        var availableWidth = Math.Max(bounds.Width - padding.Left - padding.Right, 1.0);
        var availableHeight = Math.Max(bounds.Height - padding.Top - padding.Bottom, 1.0);
        var contentWidth = Math.Max(maxX - minX, 1.0);
        var contentHeight = Math.Max(maxY - minY, 1.0);
        var scale = Math.Min(availableWidth / contentWidth, availableHeight / contentHeight);
        var offsetX = padding.Left + ((availableWidth - (contentWidth * scale)) * 0.5);
        var baselineY = bounds.Height - padding.Bottom;

        foreach (var segment in projectedSegments)
        {
            var start = new Point(
                offsetX + ((segment.Start.X - minX) * scale),
                baselineY - ((segment.Start.Y - minY) * scale));
            var end = new Point(
                offsetX + ((segment.End.X - minX) * scale),
                baselineY - ((segment.End.Y - minY) * scale));

            var pen = new Pen(segment.Brush, segment.Thickness)
            {
                StartLineCap = PenLineCap.Round,
                EndLineCap = PenLineCap.Round
            };

            drawingContext.DrawLine(pen, start, end);
        }
    }

    private void DrawMessage(DrawingContext drawingContext, Rect bounds, string message)
    {
        var formattedText = new FormattedText(
            message,
            CultureInfo.CurrentUICulture,
            FlowDirection.LeftToRight,
            new Typeface(new FontFamily("Bahnschrift"), FontStyles.Normal, FontWeights.Medium, FontStretches.Normal),
            16.0,
            MessageBrush,
            VisualTreeHelper.GetDpi(this).PixelsPerDip)
        {
            MaxTextWidth = Math.Max(bounds.Width - 64.0, 80.0),
            TextAlignment = TextAlignment.Center
        };

        var origin = new Point(
            (bounds.Width - formattedText.Width) * 0.5,
            (bounds.Height - formattedText.Height) * 0.5);

        drawingContext.DrawText(formattedText, origin);
    }
}
