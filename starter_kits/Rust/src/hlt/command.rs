use hlt::direction::Direction;
use hlt::ShipId;

pub struct Command(pub String);

impl Command {
    pub fn spawn_ship() -> Command {
        Command(format!("g"))
    }

    pub fn transform_ship_into_dropoff_site(ship_id: ShipId) -> Command {
        Command(format!("c {}", ship_id.0))
    }

    pub fn move_ship(ship_id: ShipId, direction: Direction) -> Command {
        Command(format!("m {} {}", ship_id.0, direction.get_char_encoding()))
    }
}
