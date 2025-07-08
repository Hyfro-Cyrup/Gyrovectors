using System;
using System.Diagnostics;
using System.Drawing;
using System.Numerics;
using System.Reflection.Metadata;
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

public struct GraphSegment
{
    public MöbiusGyrovector start, stop;
    public int depth;
}

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public static (double, double) Center = (200, 200);
    public static double Scale = 150;
    public static double LineWeight = 2;
    private static double PanSpeed = 0.02;
    private static List<(MöbiusGyrovector, MöbiusGyrovector)> Lines = new();

    public MainWindow()
    {
        InitializeComponent();
        this.KeyDown += OnKeyDown;
        this.SizeChanged += OnSizeChanged;

        double side_length = Math.Sqrt(2) * Math.Sin(Math.PI / 12) / Math.Sin(3 * Math.PI / 4);

        int max_depth = 5;

        for (int i = 0; i < 6; i++)
        {
            MöbiusGyrovector a = new(side_length * Math.Cos(Math.PI * i / 3), side_length * Math.Sin(Math.PI * i / 3));
            MöbiusGyrovector b = new(side_length * Math.Cos(Math.PI * (i+1) / 3), side_length * Math.Sin(Math.PI * (i+1) / 3));
            Lines.Add((a, b));
        }

        List<GraphSegment> Tier_One_Perps = new();
        
        // add tier 1 perpindiculars
        for (int i = 0; i < 6; i++)
        {
            var (a, b) = Lines[i];

            foreach (var (start, end) in new[] { (a, b), (b, a) }) // Both directions
            {
                MöbiusGyrovector prev = end; // the preceeding vector in the line gets rotated to become the perpindicular
                for (int step = 2; step < max_depth + 2; step++) // create two points in this direction
                {
                    var point = start + step * (-start + end);

                    var perp = MöbiusGyrovector.RotateAround(prev, point, Math.PI/2);

                    Tier_One_Perps.Add(new GraphSegment{ start=point, stop=perp, depth=step - 1});
                    prev = point;
                }
            }

        }

        // add tier 2+ perpindiculars
        List<GraphSegment> Tier_Two_Perps = new();
        foreach (GraphSegment graphSegment in Tier_One_Perps)
        {
            MöbiusGyrovector a = graphSegment.start, b = graphSegment.stop;
            MöbiusGyrovector prev = a;
            int depth_to_go = 1 + max_depth - graphSegment.depth;
            for (int step = 1; step < depth_to_go; step++) 
            {
                var point = a + step * (-a + b);

                var perp = MöbiusGyrovector.RotateAround(prev, point, Math.PI / 2);

                Tier_Two_Perps.Add(new GraphSegment { start=point, stop=perp, depth = graphSegment.depth + step});
                prev = point;
            }
            prev = a;
            for (int step = -1; step > -depth_to_go; step--)
            {
                var point = a + step * (-a + b);

                var perp = MöbiusGyrovector.RotateAround(prev, point, Math.PI / 2);

                Tier_Two_Perps.Add(new GraphSegment { start = point, stop = perp, depth = graphSegment.depth + step });
                prev = point;
            }
        }

        Lines.AddRange(from seg in Tier_One_Perps select (seg.start, seg.stop));
        Lines.AddRange(from seg in Tier_Two_Perps select (seg.start, seg.stop));

        Redraw();
    }

    #region Draw Tools
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
        double A = (MöbiusGyrovector.ComplexNormSquared(a) - MöbiusGyrovector.ComplexNormSquared(b)) / (2 * (a.x - b.x));
        double B = (b.y - a.y) / (a.x - b.x);
        double center_y = (MöbiusGyrovector.ComplexNormSquared(a) - 2 * A * a.x + 1) / (2 * (B * a.x + a.y));
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
    #endregion

    #region Handlers
    private void OnKeyDown(object sender, KeyEventArgs e)
    {
        MöbiusGyrovector translation = new MöbiusGyrovector(0, 0);

        switch (e.Key)
        {
            case Key.Up:
                translation = new MöbiusGyrovector(0, -PanSpeed);
                break;
            case Key.Down:
                translation = new MöbiusGyrovector(0, PanSpeed);
                break;
            case Key.Left:
                translation = new MöbiusGyrovector(PanSpeed, 0);
                break;
            case Key.Right:
                translation = new MöbiusGyrovector(-PanSpeed, 0);
                break;
        }


        Translate(translation);
        Redraw();
    }

    private void OnSizeChanged(object sender, SizeChangedEventArgs e)
    {
        double width = MyCanvas.ActualWidth;
        double height = MyCanvas.ActualHeight;

        Center = (width / 2, height / 2);
        Scale = Math.Min(width, height) * 0.4;

        Redraw();
    }

    #endregion

    private void Translate(MöbiusGyrovector ds)
    {
        for (int i = 0; i < Lines.Count; i++)
        {
            Lines[i] = (ds + Lines[i].Item1, ds + Lines[i].Item2);
        }
    }

    private void Redraw()
    {
        MyCanvas.Children.Clear();
        DrawCircle(0, 0, 1);
        foreach (var line in Lines)
        {
            DrawLine(line.Item1, line.Item2);
        }
    }


}