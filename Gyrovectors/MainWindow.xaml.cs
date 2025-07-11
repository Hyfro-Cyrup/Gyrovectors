using Gyrovectors.Models;
using Gyrovectors.ViewModels;
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

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public MainWindow()
    {
        DataContext = new PoincareDiskViewModel();
        InitializeComponent();
    }

}