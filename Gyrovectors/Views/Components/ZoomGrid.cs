using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace Gyrovectors.Views.Components;

/// <summary>
/// Container that changes the Width and Height properties of its children. Didn't use a scale transform because I didn't want to affect line weights. 
/// Kinda jank, but the only thing it needs to hold right now is a PoincareDiskView.
/// </summary>
public class ZoomGrid : Grid
{
    public DependencyProperty ZoomFactorProperty =
        DependencyProperty.Register(nameof(ZoomFactor), typeof(double), typeof(ZoomGrid),
            new PropertyMetadata(1.1));
    public double ZoomFactor
    {
        get => (double)GetValue(ZoomFactorProperty); 
        set => SetValue(ZoomFactorProperty, value); 
    }

    public ZoomGrid()
    {
        PreviewMouseWheel += OnMouseWheelZoom;
    }

    private void OnMouseWheelZoom(object sender, MouseWheelEventArgs e)
    {
        
        // Optional: Only zoom when Ctrl is held
        if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
        {

            foreach (var child in Children)
            {
                if (child is FrameworkElement el)
                {
                    el.Width = e.Delta > 0 ? el.Width * ZoomFactor : el.Width / ZoomFactor;
                    el.Height = e.Delta > 0 ? el.Height * ZoomFactor : el.Height / ZoomFactor;
                }
            }

            e.Handled = true;
        }
    }
}

