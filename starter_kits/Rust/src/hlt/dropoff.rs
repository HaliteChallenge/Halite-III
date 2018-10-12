use hlt::DropoffId;
use hlt::entity::Entity;
use hlt::input::Input;
use hlt::PlayerId;
use hlt::position::Position;

pub struct Dropoff {
    pub owner: PlayerId,
    pub id: DropoffId,
    pub position: Position,
}

impl Entity for Dropoff {
    fn owner(&self) -> PlayerId {
        self.owner
    }

    fn position(&self) -> Position {
        self.position
    }
}

impl Dropoff {
    pub fn generate(input: &mut Input, player_id: PlayerId) -> Dropoff {
        input.read_and_parse_line();
        let id = DropoffId(input.next_usize());
        let x = input.next_i32();
        let y = input.next_i32();

        Dropoff { owner: player_id, id, position: Position { x, y } }
    }
}
