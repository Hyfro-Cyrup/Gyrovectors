using CommunityToolkit.Mvvm.Input;
using Gyrovectors.Models;
using System.Collections.ObjectModel;
using System.Windows.Input;

using Gyrovectors.Utils;
using System.Runtime.CompilerServices;
using System.Diagnostics;

namespace Gyrovectors.ViewModels;

public class PoincareDiskViewModel
{
    public ObservableCollection<MöbiusGyroline> Lines { get; set; }
    public HexGrid Grid { get; set; }
    public ICommand MoveCommand { get; }
    public ICommand ToggleRecenterCommand { get; }
    private bool _shouldRecenter = true;

    public PoincareDiskViewModel()
    {
        Lines = [];
        Grid = new HexGrid();
        MoveCommand = new RelayCommand<Direction>(HandleMove);
        ToggleRecenterCommand = new RelayCommand(() =>
        {
            _shouldRecenter = !_shouldRecenter;
            if (_shouldRecenter)
            {
                RecenterIfNecessary();
                OnDataChange();
            }
        });

        // create some lines
        var length = MöbiusGyrovector.UNIT_LENGTH / 2;
        MöbiusGyrovector sw = new(-length, -length);
        MöbiusGyrovector se = new(length, -length);
        MöbiusGyrovector ne = new(length, length);
        MöbiusGyrovector nw = new(-length, length);
        Lines.Add(new(sw, se));
        Lines.Add(new(se, ne));
        Lines.Add(new(ne, nw));
        Lines.Add(new(nw, sw));
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
            Direction.Up => new MöbiusGyrovector(0, -PanSpeed),
            Direction.Down => new MöbiusGyrovector(0, PanSpeed),
            Direction.Left => new MöbiusGyrovector(PanSpeed, 0),
            Direction.Right => new MöbiusGyrovector(-PanSpeed, 0),
            _ => throw new NotSupportedException()
        };

        TranslateAll(translation);
        if (_shouldRecenter)
        {
            RecenterIfNecessary();    
        }

        OnDataChange();

    }

    private void TranslateAll(MöbiusGyrovector translation)
        => TransformAll(translation, 0.0, MöbiusGyrovector.Zero);

    private void TransformAll(MöbiusGyrovector translation, double rotation, MöbiusGyrovector around)
    {
        TransformGrid(translation, rotation, around);
        TransformLines(translation, rotation, around);
    }

    private void TransformGrid(MöbiusGyrovector translation, double rotation, MöbiusGyrovector around)
    {
        for (int i = 0; i < Grid.Lines.Count; i++)
        {
            Grid.Lines[i] = new MöbiusGyroline(MöbiusGyrovector.RotateAround(translation + Grid.Lines[i].a, around, rotation),
                MöbiusGyrovector.RotateAround(translation + Grid.Lines[i].b, around, rotation));
        }
    }

    private void TransformLines(MöbiusGyrovector translation, double rotation, MöbiusGyrovector around)
    {
        for (int i = 0; i < Lines.Count; i++)
        {
            Lines[i] = new MöbiusGyroline(MöbiusGyrovector.RotateAround(translation + Lines[i].a, around, rotation),
                MöbiusGyrovector.RotateAround(translation + Lines[i].b, around, rotation));
        }
    }

    /// <summary>
    /// Recursively transforms the grid until the primary hex surrounds the origin
    /// </summary>
    private void RecenterIfNecessary()
    {
        for (int i = 0; i < 6; i++)
        {
            var line = Grid.Lines[i];
            // if b is not counteclockwise of a relative to the origin, that means we're outside the line
            if (MöbiusGyrovector.isClockwise(line.a, line.b))
            {
                int opp_index = i < 3 ? i + 3 : i - 3;
                var opp_line = Grid.Lines[opp_index];
                var translation = line.a.BoxMinus(opp_line.b);
                var rotation = MöbiusGyrovector.Angle(translation + opp_line.a, line.b, line.a);

                TransformGrid(translation, rotation, line.a);
                RecenterIfNecessary();
                break;
            }
        }
    }
}

