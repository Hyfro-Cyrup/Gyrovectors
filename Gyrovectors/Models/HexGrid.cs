using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Gyrovectors.Models;

public class HexGrid
{
    private  struct GraphSegment
    {
        public MöbiusGyrovector start, stop;
        public int depth;
    }

    public List<MöbiusGyroline> Lines;
    private static readonly double side_length = Math.Sqrt(2) * Math.Sin(Math.PI / 12) / Math.Sin(3 * Math.PI / 4);

    public HexGrid()
    {
        Lines = [];
        for (int i = 0; i < 6; i++)
        {
            MöbiusGyrovector a = new(side_length * Math.Cos(Math.PI * i / 3), side_length * Math.Sin(Math.PI * i / 3));
            MöbiusGyrovector b = new(side_length * Math.Cos(Math.PI * (i + 1) / 3), side_length * Math.Sin(Math.PI * (i + 1) / 3));
            Lines.Add(new(a, b));
        }

        int max_depth = 5;

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

                    var perp = MöbiusGyrovector.RotateAround(prev, point, Math.PI / 2);

                    Tier_One_Perps.Add(new GraphSegment { start = point, stop = perp, depth = step - 1 });
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

                Tier_Two_Perps.Add(new GraphSegment { start = point, stop = perp, depth = graphSegment.depth + step });
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
    }

    public HexGrid(IEnumerable<MöbiusGyroline> lines)
    {
        Lines = [..lines];
    }
}
