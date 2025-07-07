using System.Diagnostics;
using System.Drawing;
using System.Numerics;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using static System.Formats.Asn1.AsnWriter;

namespace Gyrovectors;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public static (double, double) Center = (200, 200);
    public static double Scale = 150;
    public static double LineWeight = 2;

    public MainWindow()
    {
        InitializeComponent();
        DrawCircle(0, 0, 1);

        MöbiusGyrovector a = new (new(-0.5, 0.1));
        MöbiusGyrovector b = new(new(0.5, 0.1));
        SketchLine(a, b);
        DrawLine(a, b);
    }
    public void DrawCircle(double x, double y, double radius)
        => DrawCircle(x, y, radius, Brushes.Black);

    public void DrawCircle(double x, double y, double radius, Brush color)
    {
        x = Scale * x + Center.Item1;
        y = Scale * y + Center.Item2;
        radius *= Scale; 
        Ellipse lTheCircle = new Ellipse();
        lTheCircle.Width  = 2 * radius;
        lTheCircle.Height = 2 * radius;
        lTheCircle.Fill = Brushes.Transparent;
        lTheCircle.Stroke = color;
        lTheCircle.StrokeThickness = LineWeight;
        Canvas.SetTop(lTheCircle, y - radius);
        Canvas.SetLeft(lTheCircle, x - radius);
        MyCanvas.Children.Add(lTheCircle);
    }

    public void DrawPoint(double x, double y)
        => DrawPoint(x, y, Brushes.Black);
    
    public void DrawPoint(double x, double y, Brush color)
        => DrawCircle(x, y, 2/Scale, color);

    public void SketchLine(MöbiusGyrovector a, MöbiusGyrovector b)
    {
        const int COUNT = 100;
        const double RANGE = 10;

        MöbiusGyrovector ds = -a + b;

        for (double t = -RANGE; t<RANGE; t += RANGE / COUNT)
        {
            MöbiusGyrovector MyPoint = a + (ds * t);
            DrawPoint(MyPoint.x, MyPoint.y);
        }
    }

    public void DrawLine(MöbiusGyrovector a, MöbiusGyrovector b)
    {
        double A = (MöbiusGyrovector.NormSquared(a) - MöbiusGyrovector.NormSquared(b)) / (2 * (a.x - b.x));
        double B = (b.y - a.y) / (a.x - b.x);
        double center_y = (MöbiusGyrovector.NormSquared(a) - 2 * A * a.x + 1) / (2 * (B * a.x + a.y));
        double center_x = A + B * center_y;

        double radius = (new Complex(center_x - a.x, center_y - a.y)).Magnitude;

        // draw the arc
        double angle_offset = Math.Acos(1 / (new Complex(center_x, center_y).Magnitude));
        double center_angle = Math.Atan2(center_y, center_x);

        DrawArc(Math.Cos(center_angle + angle_offset), Math.Sin(center_angle + angle_offset), Math.Cos(center_angle - angle_offset), Math.Sin(center_angle - angle_offset), radius);

    }

    public void DrawArc(double start_x, double start_y, double stop_x, double stop_y, double radius)
    {
        start_x = Scale * start_x + Center.Item1;
        start_y = Scale * start_y + Center.Item2;
        stop_x = Scale * stop_x + Center.Item1;
        stop_y = Scale * stop_y + Center.Item2;
        radius *= Scale;

        Path path = new Path
        {
            Stroke = Brushes.Black,
            StrokeThickness = 2
        };

        PathFigure figure = new PathFigure
        {
            StartPoint = new System.Windows.Point(start_x, start_y)
        };

        ArcSegment arc = new ArcSegment
        {
            Point = new System.Windows.Point(stop_x, stop_y),
            Size = new System.Windows.Size(radius, radius),
            SweepDirection = SweepDirection.Clockwise,
            IsLargeArc = false
        };

        figure.Segments.Add(arc);

        var geometry = new PathGeometry();
        geometry.Figures.Add(figure);

        path.Data = geometry;
        MyCanvas.Children.Add(path);


    }

    
}