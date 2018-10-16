using System;
using System.Collections.Generic;
using System.Text;

namespace MyBot.hlt
{
    class GameMap
    {
        public  int width;
        public  int height;
        public  DictionaryCell[][] cells;

    public GameMap( int width,  int height)
        {
            this.width = width;
            this.height = height;

            cells = new DictionaryCell[height][];
            for (int y = 0; y < height; ++y)
            {
                cells[y] = new DictionaryCell[width];
            }
        }

        public DictionaryCell at( Position position)
        {
             Position normalized = normalize(position);
            return cells[normalized.y][normalized.x];
        }

        public DictionaryCell at( Entity entity)
        {
            return at(entity.position);
        }

        public int calculateDistance( Position source,  Position target)
        {
             Position normalizedSource = normalize(source);
             Position normalizedTarget = normalize(target);

             int dx = Math.Abs(normalizedSource.x - normalizedTarget.x);
             int dy = Math.Abs(normalizedSource.y - normalizedTarget.y);

             int toroidal_dx = Math.Min(dx, width - dx);
             int toroidal_dy = Math.Min(dy, height - dy);

            return toroidal_dx + toroidal_dy;
        }

        public Position normalize( Position position)
        {
             int x = ((position.x % width) + width) % width;
             int y = ((position.y % height) + height) % height;
            return new Position(x, y);
        }

        public List<Direction> getUnsafeMoves( Position source,  Position destination)
        {
             List<Direction> possibleMoves = new List<Direction>();

             Position normalizedSource = normalize(source);
             Position normalizedDestination = normalize(destination);

             int dx = Math.Abs(normalizedSource.x - normalizedDestination.x);
             int dy = Math.Abs(normalizedSource.y - normalizedDestination.y);
             int wrapped_dx = width - dx;
             int wrapped_dy = height - dy;

            if (normalizedSource.x < normalizedDestination.x)
            {
                possibleMoves.Add(dx > wrapped_dx ? (Direction)new WEST() : new EAST());
            }
            else if (normalizedSource.x > normalizedDestination.x)
            {
                possibleMoves.Add(dx < wrapped_dx ? (Direction)new WEST() : new EAST());
            }

            if (normalizedSource.y < normalizedDestination.y)
            {
                possibleMoves.Add(dy > wrapped_dy ? (Direction)new NORTH() : new SOUTH());
            }
            else if (normalizedSource.y > normalizedDestination.y)
            {
                possibleMoves.Add(dy < wrapped_dy ? (Direction)new NORTH() : new SOUTH());
            }

            return possibleMoves;
        }

        public Direction naiveNavigate( Ship ship,  Position destination)
        {
            // getUnsafeMoves normalizes for us
            foreach ( Direction direction in getUnsafeMoves(ship.position, destination))
            {
                 Position targetPos = ship.position.directionalOffset(direction);
                if (!at(targetPos).isOccupied())
                {
                    at(targetPos).markUnsafe(ship);
                    return direction;
                }
            }

            return new STILL();
        }

        public void _update()
        {
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    cells[y][x].ship = null;
                }
            }

             int updateCount = Input.readInput().getInt();

            for (int i = 0; i < updateCount; ++i)
            {
                 Input input = Input.readInput();
                 int x = input.getInt();
                 int y = input.getInt();

                cells[y][x].halite = input.getInt();
            }
        }

        public static GameMap _generate()
        {
             Input DictionaryInput = Input.readInput();
             int width = DictionaryInput.getInt();
             int height = DictionaryInput.getInt();

             GameMap Dictionary = new GameMap(width, height);

            for (int y = 0; y < height; ++y)
            {
                 Input rowInput = Input.readInput();

                for (int x = 0; x < width; ++x)
                {
                     int halite = rowInput.getInt();
                    Dictionary.cells[y][x] = new DictionaryCell(new Position(x, y), halite);
                }
            }

            return Dictionary;
        }
    }
}
