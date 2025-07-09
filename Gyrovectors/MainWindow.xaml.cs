using System;
using System.Diagnostics;
using System.Drawing;
using System.Numerics;
using System.Reflection.Metadata;
using System.Text;
using System.Transactions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
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
    private static List<MöbiusGyroline> Lines = new();
    private static MöbiusGyrovector min_point = MöbiusGyrovector.Zero, opposite_point = MöbiusGyrovector.Zero;
    private static int min_index = -1;
    private static bool flag = false;

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
            Lines.Add(new(a, b));
        }

        List<GraphSegment> Tier_One_Perps = new();
        
        // add tier 1 perpindiculars
        for (int i = 0; i < 6; i++)
        {
            var line = Lines[i];

            foreach (var (start, end) in new[] { (line.a, line.b), (line.b, line.a) }) // Both directions
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

        Lines.AddRange(from seg in Tier_One_Perps select new MöbiusGyroline(seg.start, seg.stop));
        Lines.AddRange(from seg in Tier_Two_Perps select new MöbiusGyroline(seg.start, seg.stop));

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
    => DrawLine(new MöbiusGyroline(a, b));

    public void DrawLine(MöbiusGyroline line)
    {
        double angle_offset = Math.Acos(1 / line.center.Magnitude);
        double center_angle = Math.Atan2(line.center.Imaginary, line.center.Real);

        DrawArc(Math.Cos(center_angle + angle_offset), Math.Sin(center_angle + angle_offset), Math.Cos(center_angle - angle_offset), Math.Sin(center_angle - angle_offset), line.radius);
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
        MöbiusGyrovector translation = MöbiusGyrovector.Zero;
        double rotation = 0;

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
        RecenterIfNecessary();
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

    private void Translate(MöbiusGyrovector translation)
        => Transform(translation, 0.0, MöbiusGyrovector.Zero);

    private void Transform(MöbiusGyrovector translation, double rotation, MöbiusGyrovector around)
    {
        if (flag)
        {
            Debug.WriteLine(("around: ", around));
        }
        for (int i = 0; i < Lines.Count; i++)
        {
            Lines[i] = new MöbiusGyroline(MöbiusGyrovector.RotateAround(translation + Lines[i].a, around, rotation),
                MöbiusGyrovector.RotateAround(translation + Lines[i].b, around, rotation));
        }
    }

    private void RecenterIfNecessary()
    {
        // check if `ds` just took us across one of the 6 main line segments
        // Note this assumes this method is called directly after Transform()
        List<int> crossings = new List<int>();
        for (int i = 0; i < 6; i++)
        {
            // if the k component of the cross product a x b is positive, then b is counteclockwise of a relative to the origin (that means we're inside the line)
            if (Lines[i].a.x * Lines[i].b.y - Lines[i].a.y * Lines[i].b.x < 0) 
            {
                crossings.Add(i); 
            }
        }
        foreach (int i in crossings)
        {
            // we either crossed one line or we crossed two. Doesn't matter. We can just recenter twice.
            var line = Lines[i];
            int opp_index = i < 3 ? i + 3 : i - 3;
            var opp_line = Lines[opp_index];
            var translation = line.a.BoxMinus(opp_line.b);
            var rotation = MöbiusGyrovector.Angle(translation + opp_line.a, line.b, line.a);
            
            Transform(translation, rotation, line.a);
        }

    }

    private void Redraw()
    {
        MyCanvas.Children.Clear();
        DrawCircle(0, 0, 1);
        DrawPoint(0, 0, Brushes.Red);
        foreach (var line in Lines)
        {
            DrawLine(line.a, line.b);
        }

        // Get the point on OG hexagon that is closest to the origin. 
        double min_distance = 9999;
        double distance;
        MöbiusGyrovector point;
        for (int i = 0; i < 6; i++)
        {
            point = MöbiusGyrovector.NearestPointOnLine(MöbiusGyrovector.Zero, Lines[i]);
            distance = MöbiusGyrovector.Distance(MöbiusGyrovector.Zero, point);
            if (distance < min_distance)
            {
                min_distance = distance;
                min_point = point;
                min_index = i;
            }
        }

        DrawPoint(min_point.x, min_point.y, Brushes.Red);

        // try to find the opposite point
        int antipodal_index = min_index < 3 ? min_index + 3 : min_index - 3;
        double t = MöbiusGyrovector.Distance(min_point, Lines[min_index].a) / MöbiusGyrovector.Distance(Lines[min_index].b, Lines[min_index].a);
        opposite_point = Lines[antipodal_index].b + t*(-Lines[antipodal_index].b + Lines[antipodal_index].a);
        DrawPoint(opposite_point.x, opposite_point.y, Brushes.Green);

        //TODO: Move this all somewhere reasonable (centralize list of points to draw)
        //TODO: We don't actually need to translate opposite point -> point. We need to translate opposite a -> a and rotate opposite b -> b
    }


}