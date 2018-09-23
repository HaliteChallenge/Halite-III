use hlt::EntityId;
use hlt::PlayerId;
use hlt::position::Position;

pub trait Entity {
    fn owner(&self) -> PlayerId;
    fn id(&self) -> EntityId;
    fn position(&self) -> Position;
}
