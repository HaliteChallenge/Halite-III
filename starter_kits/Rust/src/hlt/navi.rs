use hlt::direction::Direction;
use hlt::position::Position;
use hlt::ship::Ship;
use hlt::ShipId;
use hlt::game::Game;

pub struct Navi {
    pub width: usize,
    pub height: usize,
    pub occupied: Vec<Vec<Option<ShipId>>>,
}

impl Navi {
    pub fn new(width: usize, height: usize) -> Navi {
        let mut occupied: Vec<Vec<Option<ShipId>>> = Vec::with_capacity(height);
        for _ in 0..height {
            occupied.push(vec![None; width]);
        }

        Navi { width, height, occupied }
    }

    pub fn update_frame(&mut self, game: &Game) {
        self.clear();

        for player in &game.players {
            for ship_id in &player.ship_ids {
                let ship = &game.ships[ship_id];
                self.mark_unsafe_ship(&ship);
            }
        }
    }

    pub fn clear(&mut self) {
        for y in 0..self.height {
            for x in 0..self.width {
                self.occupied[y][x] = None;
            }
        }
    }

    pub fn is_safe(&self, position: &Position) -> bool {
        let position = self.normalize(position);
        self.occupied[position.y as usize][position.x as usize].is_none()
    }

    pub fn is_unsafe(&self, position: &Position) -> bool {
        !self.is_safe(position)
    }

    pub fn mark_unsafe(&mut self, position: &Position, ship_id: ShipId) {
        let position = self.normalize(position);
        self.occupied[position.y as usize][position.x as usize] = Some(ship_id);
    }

    pub fn mark_unsafe_ship(&mut self, ship: &Ship) {
        self.mark_unsafe(&ship.position, ship.id);
    }

    pub fn get_unsafe_moves(&self, source: &Position, destination: &Position) -> Vec<Direction> {
        let normalized_source = self.normalize(source);
        let normalized_destination = self.normalize(destination);

        let dx = (normalized_source.x - normalized_destination.x).abs() as usize;
        let dy = (normalized_source.y - normalized_destination.y).abs() as usize;

        let wrapped_dx = self.width - dx;
        let wrapped_dy = self.height - dy;

        let mut possible_moves: Vec<Direction> = Vec::new();

        if normalized_source.x < normalized_destination.x {
            possible_moves.push(if dx > wrapped_dx { Direction::West } else { Direction::East });
        } else if normalized_source.x > normalized_destination.x {
            possible_moves.push(if dx < wrapped_dx { Direction::West } else { Direction::East });
        }

        if normalized_source.y < normalized_destination.y {
            possible_moves.push(if dy > wrapped_dy { Direction::North } else { Direction::South });
        } else if normalized_source.y > normalized_destination.y {
            possible_moves.push(if dy < wrapped_dy { Direction::North } else { Direction::South });
        }

        possible_moves
    }

    pub fn naive_navigate(&mut self, ship: &Ship, destination: &Position) -> Direction {
        let ship_position = &ship.position;

        // get_unsafe_moves normalizes for us
        for direction in self.get_unsafe_moves(&ship_position, destination) {
            let target_pos = ship_position.directional_offset(direction);

            if self.is_safe(&target_pos) {
                self.mark_unsafe(&target_pos, ship.id);
                return direction;
            }
        }

        Direction::Still
    }

    pub fn normalize(&self, position: &Position) -> Position {
        let width = self.width as i32;
        let height = self.height as i32;
        let x = ((position.x % width) + width) % width;
        let y = ((position.y % height) + height) % height;
        Position { x, y }
    }
}
