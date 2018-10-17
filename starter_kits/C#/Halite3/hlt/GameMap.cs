using System;
using System.Collections.Generic;

namespace Halite_Project.hlt {
    public class GameMap {
        public int width;
        public int height;
        public MapCell[][] cells;

        public GameMap(int width, int height) {
            this.width = width;
            this.height = height;

            cells = new MapCell[height][];
            for (int y = 0; y < height; ++y) {
                cells[y] = new MapCell[width];
            }
        }

        public MapCell At(Position position) {
            Position normalized = Normalize(position);
            return cells[normalized.y][normalized.x];
        }

        public MapCell At(Entity entity) {
            return At(entity.position);
        }

        public int CalculateDistance(Position source, Position target) {
            Position normalizedSource = Normalize(source);
            Position normalizedTarget = Normalize(target);

            int dx = Math.Abs(normalizedSource.x - normalizedTarget.x);
            int dy = Math.Abs(normalizedSource.y - normalizedTarget.y);

            int toroidal_dx = Math.Min(dx, width - dx);
            int toroidal_dy = Math.Min(dy, height - dy);

            return toroidal_dx + toroidal_dy;
        }

        public Position Normalize(Position position) {
            int x = ((position.x % width) + width) % width;
            int y = ((position.y % height) + height) % height;
            return new Position(x, y);
        }

        public List<EDirection> GetUnsafeMoves(Position source, Position destination) {
            List<EDirection> possibleMoves = new List<EDirection>();

            Position normalizedSource = Normalize(source);
            Position normalizedDestination = Normalize(destination);

            int dx = Math.Abs(normalizedSource.x - normalizedDestination.x);
            int dy = Math.Abs(normalizedSource.y - normalizedDestination.y);
            int wrapped_dx = width - dx;
            int wrapped_dy = height - dy;

            if (normalizedSource.x < normalizedDestination.x) {
                possibleMoves.Add(dx > wrapped_dx ? EDirection.WEST : EDirection.EAST);
            } else if (normalizedSource.x > normalizedDestination.x) {
                possibleMoves.Add(dx < wrapped_dx ? EDirection.WEST : EDirection.EAST);
            }

            if (normalizedSource.y < normalizedDestination.y) {
                possibleMoves.Add(dy > wrapped_dy ? EDirection.NORTH : EDirection.SOUTH);
            } else if (normalizedSource.y > normalizedDestination.y) {
                possibleMoves.Add(dy < wrapped_dy ? EDirection.NORTH : EDirection.SOUTH);
            }

            return possibleMoves;
        }

        public EDirection NaiveNavigate(Ship ship, Position destination) {
            // getUnsafeMoves normalizes for us
            foreach (EDirection direction in GetUnsafeMoves(ship.position, destination)) {
                Position targetPos = ship.position.DirectionalOffset(direction);
                if (!At(targetPos).IsOccupied()) {
                    At(targetPos).MarkUnsafe(ship);
                    return direction;
                }
            }

            return EDirection.STILL;
        }

        public void _update() {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    cells[y][x].ship = null;
                }
            }

            int updateCount = Input.ReadInput().GetInt();

            for (int i = 0; i < updateCount; ++i) {
                Input input = Input.ReadInput();
                int x = input.GetInt();
                int y = input.GetInt();

                cells[y][x].halite = input.GetInt();
            }
        }

        public static GameMap _generate() {
            Input mapInput = Input.ReadInput();
            int width = mapInput.GetInt();
            int height = mapInput.GetInt();

            GameMap map = new GameMap(width, height);

            for (int y = 0; y < height; ++y) {
                Input rowInput = Input.ReadInput();

                for (int x = 0; x < width; ++x) {
                    int halite = rowInput.GetInt();
                    map.cells[y][x] = new MapCell(new Position(x, y), halite);
                }
            }

            return map;
        }
    }
}