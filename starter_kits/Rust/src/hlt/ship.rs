use hlt::command::Command;
use hlt::direction::Direction;
use hlt::entity::Entity;
use hlt::EntityId;
use hlt::input::Input;
use hlt::PlayerId;
use hlt::position::Position;

pub struct Ship {
    pub owner: PlayerId,
    pub id: EntityId,
    pub position: Position,
    pub halite: i32,
    max_halite: i32,
}

impl Ship {
    pub fn is_full(&self) -> bool {
        self.halite >= self.max_halite
    }

    pub fn make_dropoff(&self) -> Command {
        Command::transform_ship_into_dropoff_site(self.id)
    }

    pub fn move_ship(&self, direction: Direction) -> Command {
        Command::move_ship(self.id, direction)
    }

    pub fn stay_still(&self) -> Command {
        Command::move_ship(self.id, Direction::Still)
    }

    pub fn generate(input: &mut Input, player_id: PlayerId, max_halite: i32) -> Ship {
        input.read_and_parse_line();
        let id = EntityId(input.next_i32());
        let x = input.next_i32();
        let y = input.next_i32();
        let halite = input.next_i32();

        Ship { owner: player_id, id, position: Position { x, y }, halite, max_halite }
    }
}

impl Entity for Ship {
    fn owner(&self) -> PlayerId {
        self.owner
    }

    fn id(&self) -> EntityId {
        self.id
    }

    fn position(&self) -> Position {
        self.position
    }
}
