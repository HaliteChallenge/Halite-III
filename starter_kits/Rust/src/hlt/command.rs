use hlt::direction::Direction;
use hlt::ShipId;

pub struct Command(pub String);

impl Command {
    pub fn spawn_ship() -> Command {
        Command(format!("g"))
    }

    pub fn transform_ship_into_dropoff_site(id: ShipId) -> Command {
        Command(format!("c {}", id.0))
    }

    pub fn move_ship(id: ShipId, direction: Direction) -> Command {
        Command(format!("m {} {}", id.0, direction.get_char_encoding()))
    }
}
