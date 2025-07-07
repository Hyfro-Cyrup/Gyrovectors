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
    public MainWindow()
    {
        InitializeComponent();
        Canvas lmc = MyCanvas;
        Ellipse lEll = new Ellipse();
        lEll.Width = 100;
        lEll.Height = 100;
        lEll.Fill = new SolidColorBrush(Color.FromRgb(0, 200, 0));
        lmc.Children.Add(lEll);
    }

}