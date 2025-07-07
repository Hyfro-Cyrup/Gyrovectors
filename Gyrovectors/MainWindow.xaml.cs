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

namespace Gyrovectors;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public static (double, double) Center = (350, 350);
    public static double Scale = 300;
    public static double LineWeight = 2;

    public MainWindow()
    {
        InitializeComponent();
        DrawCircle(0, 0, 1);
        DrawLine(new(new(-0.5, 0.1)), new(new(0.5, 0.1)));
    }
    public void DrawCircle(double x, double y, double radius)
        => DrawCircle(x, y, radius, Brushes.Black);

    public void DrawCircle(double x, double y, double radius, Brush color)
    {
        x = Scale * x + Center.Item1;
        y = Scale * y + Center.Item2;
        radius *= Scale; 
        Ellipse lTheCircle = new Ellipse();
        lTheCircle.Width  = radius;
        lTheCircle.Height = radius;
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

    public void DrawLine(MöbiusGyrovector a, MöbiusGyrovector b)
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
}