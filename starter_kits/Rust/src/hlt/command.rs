use hlt::direction::Direction;
use hlt::EntityId;

pub struct Command(pub String);

impl Command {
    pub fn spawn_ship() -> Command {
        Command(format!("g"))
    }

    pub fn transform_ship_into_dropoff_site(id: EntityId) -> Command {
        Command(format!("c {}", id.0))
    }

    pub fn move_ship(id: EntityId, direction: Direction) -> Command {
        Command(format!("m {} {}", id.0, direction.get_char_encoding()))
    }
}
