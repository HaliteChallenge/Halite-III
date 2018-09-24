use hlt::EntityId;
use hlt::PlayerId;
use hlt::position::Position;
use hlt::entity::Entity;
use hlt::command::Command;

pub struct Shipyard {
    pub owner: PlayerId,
    pub position: Position,
}

impl Shipyard {
    pub fn spawn(&self) -> Command {
        Command::spawn_ship()
    }
}

impl Entity for Shipyard {
    fn owner(&self) -> PlayerId {
        self.owner
    }

    fn id(&self) -> EntityId {
        EntityId(-1)
    }

    fn position(&self) -> Position {
        self.position
    }
}
