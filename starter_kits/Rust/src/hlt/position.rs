use hlt::direction::Direction;

#[derive(Copy, Clone)]
pub struct Position {
    pub x: i32,
    pub y: i32,
}

impl Position {
    pub fn directional_offset(&self, d: Direction) -> Position {
        let (dx, dy) = match d {
            Direction::NORTH => (0, -1),
            Direction::SOUTH  => (0, 1),
            Direction::EAST => (1, 0),
            Direction::WEST => (-1, 0),
            Direction::STILL => (0, 0),
        };

        Position { x: self.x + dx, y: self.y + dy }
    }

    pub fn get_surrounding_cardinals(&self) -> Vec<Position> {
        vec! {
            self.directional_offset(Direction::NORTH), self.directional_offset(Direction::SOUTH),
            self.directional_offset(Direction::EAST), self.directional_offset(Direction::WEST)
        }
    }
}
