using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Gyrovectors.Views.Components;

/// <summary>
/// Clips the drawing region to an circle
/// </summary>
public class CircularCanvas : Canvas
{
    public double Radius { get; private set; }
    public Point Center;
    public CircularCanvas()
    {
        SizeChanged += (_, __) => UpdateClip();
        Loaded += (_, __) => UpdateClip();

    }

    private void UpdateClip()
    {
        // Create a new circular clipping region
        UpdateCircle();
        Clip = new EllipseGeometry
        {
            Center = Center,
            RadiusX = Radius,
            RadiusY = Radius
        };

        // Trigger OnRender
        InvalidateVisual();
    }

    #region Dependency Properties
    public static readonly DependencyProperty BorderBrushProperty =
        DependencyProperty.Register(nameof(BorderBrush), typeof(Brush), typeof(CircularCanvas),
            new FrameworkPropertyMetadata(Brushes.Black, FrameworkPropertyMetadataOptions.AffectsRender));

    public Brush BorderBrush
    {
        get => (Brush)GetValue(BorderBrushProperty);
        set => SetValue(BorderBrushProperty, value);
    }

    public static readonly DependencyProperty BorderThicknessProperty =
        DependencyProperty.Register(nameof(BorderThickness), typeof(double), typeof(CircularCanvas),
            new FrameworkPropertyMetadata(2.0, FrameworkPropertyMetadataOptions.AffectsRender));

    public double BorderThickness
    {
        get => (double)GetValue(BorderThicknessProperty);
        set => SetValue(BorderThicknessProperty, value);
    }
    #endregion


    protected override void OnRender(DrawingContext dc)
    {
        base.OnRender(dc);

        UpdateCircle();
        var radius = Radius - BorderThickness / 2;

        var pen = new Pen(BorderBrush, BorderThickness);
        dc.DrawEllipse(null, pen, Center, radius, radius);
    }
    protected void UpdateCircle()
    {
        double cx = ActualWidth / 2.0;
        double cy = ActualHeight / 2.0;
        Radius = Math.Min(cx, cy);
        Center = new Point(cx, cy);
    }
}
