using CommunityToolkit.Mvvm.Input;
using Gyrovectors.Models;
using Gyrovectors.ViewModels;
using Gyrovectors.Views.Components;
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Numerics;
using System.Security.Permissions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Gyrovectors.Views;
public class PoincareDiskView : CircularCanvas
{
    #region Data Bindings
    // NOTE: Lines is just a placeholder for Sprites or something. I don't think we actually need a list of lines to render (other than the grid, which became self-contained because of its complicated construction)
    public static readonly DependencyProperty LinesProperty =
        DependencyProperty.Register(nameof(Lines), typeof(IEnumerable<MöbiusGyroline>), typeof(PoincareDiskView),
            new FrameworkPropertyMetadata(null));

    public static readonly DependencyProperty GridProperty =
        DependencyProperty.Register(nameof(Grid), typeof(HexGrid), typeof(PoincareDiskView),
            new PropertyMetadata(null));

    public IEnumerable<MöbiusGyroline> Lines
    {
        get => (IEnumerable<MöbiusGyroline>)GetValue(LinesProperty);
        set => SetValue(LinesProperty, value);
    }

    public HexGrid Grid
    {
        get => (HexGrid)GetValue(GridProperty);
        set => SetValue(GridProperty, value);
    }

    // This is the cheese. We're not currently listening to PropertyChanged events because the only time anything changes, everything changes
    // Instead, we're listening to DataChanged events created by PoincareDiskViewModel
    private void OnLoaded(object sender, RoutedEventArgs e)
    {
        if (DataContext is PoincareDiskViewModel vm)
        {
            vm.DataChanged += (_, __) => Render();
        }
        Render();
    }
    #endregion

    public PoincareDiskView()
    {
        foreach (var pen in Pens)
        {
            pen.Freeze();
        }
        Loaded += OnLoaded;
        SizeChanged += (_, __) => Render();
    }

    #region Pens
    private static readonly Pen Base_Pen = new Pen(Brushes.Black, 1);
    private static readonly Pen Red_Pen = new Pen(new SolidColorBrush(Color.FromArgb(255, 255, 0, 0)), 2);     // Red
    private static readonly Pen Green_Pen = new Pen(new SolidColorBrush(Color.FromArgb(255, 0, 255, 0)), 2);     // Green
    private static readonly Pen Blue_Pen = new Pen(new SolidColorBrush(Color.FromArgb(255, 0, 0, 255)), 2);     // Blue
    private static readonly Pen Orange_Pen = new Pen(new SolidColorBrush(Color.FromArgb(255, 255, 165, 0)), 2);   // Orange
    private static readonly Pen Purple_Pen = new Pen(new SolidColorBrush(Color.FromArgb(255, 128, 0, 128)), 2);   // Purple
    private static readonly Pen Cyan_Pen = new Pen(new SolidColorBrush(Color.FromArgb(255, 0, 255, 255)), 2);   // Cyan

    private static readonly List<Pen> Pens = [Base_Pen, Red_Pen, Green_Pen, Blue_Pen, Orange_Pen, Purple_Pen, Cyan_Pen];
    #endregion

    #region Rendering
    private readonly DrawingGroup backingStore = new(); // Lets us draw without triggering layout updates and such
    protected override void OnRender(DrawingContext dc)
    {
        
        base.OnRender(dc);

        dc.DrawDrawing(backingStore);
        return;
    }

    // Never call InvalidateLayout; call Render instead. It will draw without triggering layout updates.
    public void Render()
    {
        Children.Clear();
        var drawingContext = backingStore.Open();
        Render(drawingContext);
        drawingContext.Close();
    }

    public void Render(DrawingContext dc)
    {
        bool firstflag = true;
        foreach (var line in Grid.Lines)
        {
            if (firstflag)
            {
                firstflag = false;
            }
            DrawGyroline(line, dc, Base_Pen);
        }


        foreach (var line in Lines)
        {
            DrawGyroSegment(line, dc, Base_Pen);
        }

        // Highlight the origin square
        for (int i = 0; i < 6; i++)
        {
            DrawGyroSegment(Grid.Lines[i], dc, Red_Pen);
            DrawGyrovector(Grid.Lines[i].a, dc, Pens[i +1], 2);
        }
        DrawGyrovector(MöbiusGyrovector.Zero, dc, Cyan_Pen);

        DrawImage(Lines, dc);
    }
    #endregion

    #region Convert Coordinates
    private Point World2Screen(double x, double y) // convert points
    {
        return new(Center.X + Radius*x,  Center.Y - Radius*y);
    }
    private Point World2Screen(Complex z) => World2Screen(z.Real, z.Imaginary);
    private Point World2Screen(MöbiusGyrovector a) => World2Screen(a.x, a.y);
    private double World2Screen(double r) // convert magnitudes
        => Radius * r;
    #endregion

    #region Draw Tools
    private void DrawGyroline(MöbiusGyroline line, DrawingContext dc, Pen pen)
    {
        // If the line is not extreme, we can draw it as an ellipse.
        if (line.radius < 50)
        {
            var radius = World2Screen(line.radius);
            dc.DrawEllipse(null, pen, World2Screen(line.center), radius, radius);
            return;
        }

        // If the line is extreme, dc.DrawEllipse becomes inaccurate.
        // WPF approximates Ellipses and Arcs as Bezier curves. This is only ever noticable for curves with large radii and center not near the x or y axis
        // In these cases, rendering arcs ensures that the Bezier curves pass through the specified points
        double angle_offset = Math.Acos(1 / line.center.Magnitude);
        double center_angle = Math.Atan2(line.center.Imaginary, line.center.Real);

        DrawEuclideanArc(new(Math.Cos(center_angle + angle_offset), Math.Sin(center_angle + angle_offset)), new(Math.Cos(center_angle - angle_offset), Math.Sin(center_angle - angle_offset)), line.radius, dc, pen);
        
    }

    private void DrawGyroSegment(MöbiusGyroline line, DrawingContext dc, Pen pen)
        => DrawEuclideanArc(line.a, line.b, line.radius, dc, pen);

    private void DrawEuclideanArc(MöbiusGyrovector Start, MöbiusGyrovector Stop, double Radius, DrawingContext dc, Pen pen)
    {
        var fig = GenerateArcPathFigure(Start, Stop, Radius);
        var geo = new PathGeometry([fig]);

        dc.DrawGeometry(null, pen, geo);
    }

    private void DrawGyrovector(MöbiusGyrovector a, DrawingContext dc, Pen pen, int weight=1)
    {
        dc.DrawEllipse(pen.Brush, pen, World2Screen(a), weight, weight);
    }

    private ArcSegment GenerateArcSegment(MöbiusGyrovector Start, MöbiusGyrovector Stop, double Radius)
    {
        var radius = World2Screen(Radius);
        return new ArcSegment(
            World2Screen(Stop),
            new Size(radius, radius),
            0,
            false,
            MöbiusGyrovector.isClockwise(Start, Stop) ? SweepDirection.Counterclockwise : SweepDirection.Clockwise,
            true
            );
    }

    private PathFigure GenerateArcPathFigure(MöbiusGyrovector Start, MöbiusGyrovector Stop, double Radius)
    {
        return new PathFigure(World2Screen(Start), [GenerateArcSegment(Start, Stop, Radius)], false);
    }

    // four_lines must be 4 lines sharing endpoints oriented CCW
    private void DrawImage(IEnumerable<MöbiusGyroline> four_lines, DrawingContext dc)
    {
        var fig = new PathFigure(World2Screen(four_lines.First().a), from line in four_lines select GenerateArcSegment(line.a, line.b, line.radius), true);
        var geo = new PathGeometry([fig]);
        var image = new BitmapImage(new Uri("https://i.sstatic.net/tekbA.jpg"));
        var imageBrush = new ImageBrush(image)
        {
            Stretch = Stretch.Fill // or Uniform / UniformToFill depending on desired mapping
        };
        dc.DrawGeometry(imageBrush, null, geo);

    }

    #endregion

}
