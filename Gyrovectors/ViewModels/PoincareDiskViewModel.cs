using CommunityToolkit.Mvvm.Input;
using Gyrovectors.Models;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

using Gyrovectors.Utils;

namespace Gyrovectors.ViewModels;

public class PoincareDiskViewModel
{
    public ObservableCollection<MöbiusGyroline> Lines { get; set; }
    public HexGrid Grid { get; set; }
    public ICommand MoveCommand { get; }

    public PoincareDiskViewModel()
    {
        Lines = [];
        Grid = new HexGrid();
        MoveCommand = new RelayCommand<Direction>(HandleMove);
    }


    public event EventHandler? DataChanged;
    protected void OnDataChange()
    {
        DataChanged?.Invoke(this, EventArgs.Empty);
    }

    public void HandleMove(Direction direction)
    {
        double PanSpeed = 0.01;
        MöbiusGyrovector translation = direction switch
        {
            Direction.Up => new MöbiusGyrovector(0, PanSpeed),
            Direction.Down => new MöbiusGyrovector(0, -PanSpeed),
            Direction.Left => new MöbiusGyrovector(PanSpeed, 0),
            Direction.Right => new MöbiusGyrovector(-PanSpeed, 0),
            _ => throw new NotSupportedException()
        };

        Translate(translation);
        RecenterIfNecessary();

        OnDataChange();

    }

    private void Translate(MöbiusGyrovector translation)
        => Transform(translation, 0.0, MöbiusGyrovector.Zero);

    private void Transform(MöbiusGyrovector translation, double rotation, MöbiusGyrovector around)
    {
        for (int i = 0; i < Grid.Lines.Count; i++)
        {
            Grid.Lines[i] = new MöbiusGyroline(MöbiusGyrovector.RotateAround(translation + Grid.Lines[i].a, around, rotation),
                MöbiusGyrovector.RotateAround(translation + Grid.Lines[i].b, around, rotation));
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
        List<int> crossings = [];
        for (int i = 0; i < 6; i++)
        {
            // if the k component of the cross product a x b is positive, then b is counteclockwise of a relative to the origin (that means we're inside the line)
            if (Grid.Lines[i].a.x * Grid.Lines[i].b.y - Grid.Lines[i].a.y * Grid.Lines[i].b.x < 0)
            {
                crossings.Add(i);
            }
        }
        foreach (int i in crossings)
        {
            // we either crossed one line or we crossed two. Doesn't matter. We can just recenter twice.
            var line = Grid.Lines[i];
            int opp_index = i < 3 ? i + 3 : i - 3;
            var opp_line = Grid.Lines[opp_index];
            var translation = line.a.BoxMinus(opp_line.b);
            var rotation = MöbiusGyrovector.Angle(translation + opp_line.a, line.b, line.a);

            Transform(translation, rotation, line.a);
        }

    }
}

