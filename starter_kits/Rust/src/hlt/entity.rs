use hlt::PlayerId;
use hlt::position::Position;

pub trait Entity {
    fn owner(&self) -> PlayerId;
    fn position(&self) -> Position;
}
