use hlt::DropoffId;
use hlt::position::Position;
use hlt::ShipId;
use hlt::PlayerId;

pub struct MapCell {
    pub position: Position,
    pub halite: usize,
    pub ship: Option<ShipId>,
    pub structure: Structure,
}

impl MapCell {
    pub fn is_empty(&self) -> bool {
        self.ship.is_none() && self.structure.is_none()
    }

    pub fn is_occupied(&self) -> bool {
        self.ship.is_some()
    }

    pub fn has_structure(&self) -> bool {
        self.structure.is_some()
    }

    pub fn mark_unsafe(&mut self, ship_id: ShipId) {
        self.ship = Some(ship_id);
    }
}

#[derive(Eq, PartialEq)]
pub enum Structure {
    None,
    Dropoff(DropoffId),
    Shipyard(PlayerId),
}

impl Structure {
    pub fn is_some(&self) -> bool {
        match *self {
            Structure::None => false,
            _ => true,
        }
    }

    pub fn is_none(&self) -> bool {
        !self.is_some()
    }
}
