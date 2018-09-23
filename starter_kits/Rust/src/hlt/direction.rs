#[derive(Copy, Clone)]
pub enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    STILL,
}

impl Direction {
    pub fn invert_direction(&self) -> Direction {
        match self {
            Direction::NORTH => Direction::SOUTH,
            Direction::EAST => Direction::WEST,
            Direction::SOUTH  => Direction::NORTH,
            Direction::WEST => Direction::EAST,
            Direction::STILL => Direction::STILL,
        }
    }

    pub fn get_all_cardinals() -> Vec<Direction> {
        vec![Direction::NORTH, Direction::SOUTH, Direction::EAST, Direction::WEST]
    }

    pub fn get_char_encoding(&self) -> char {
        match self {
            Direction::NORTH => 'n',
            Direction::EAST => 'e',
            Direction::SOUTH  => 's',
            Direction::WEST => 'w',
            Direction::STILL => 'o',
        }
    }
}
