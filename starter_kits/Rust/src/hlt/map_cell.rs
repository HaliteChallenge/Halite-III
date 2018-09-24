use hlt::entity::Entity;
use hlt::position::Position;
use hlt::ship::Ship;
use std::rc::Rc;

pub struct MapCell {
    pub position: Position,
    pub halite: i32,
    pub ship: Option<Rc<Ship>>,
    pub structure: Option<Rc<Entity>>,
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

    pub fn mark_unsafe(&mut self, ship: &Rc<Ship>) {
        self.ship = Some(ship.clone());
    }
}
