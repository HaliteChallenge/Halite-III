use hlt::dropoff::Dropoff;
use hlt::DropoffId;
use hlt::input::Input;
use hlt::PlayerId;
use hlt::position::Position;
use hlt::ship::Ship;
use hlt::ShipId;
use hlt::shipyard::Shipyard;
use std::collections::HashMap;

pub struct Player {
    pub id: PlayerId,
    pub shipyard: Shipyard,
    pub halite: usize,
    pub ship_ids: Vec<ShipId>,
    pub dropoff_ids: Vec<DropoffId>,
}

impl Player {
    pub fn update(
        &mut self,
        input: &mut Input,
        max_halite: usize,
        ships: &mut HashMap<ShipId, Ship>,
        dropoffs: &mut HashMap<DropoffId, Dropoff>,
        num_ships: usize,
        num_dropoffs: usize,
        halite: usize)
    {
        self.halite = halite;

        self.ship_ids.clear();
        for _ in 0..num_ships {
            let ship = Ship::generate(input, self.id, max_halite);
            self.ship_ids.push(ship.id);
            ships.insert(ship.id, ship);
        }

        self.dropoff_ids.clear();
        for _ in 0..num_dropoffs {
            let dropoff = Dropoff::generate(input, self.id);
            self.dropoff_ids.push(dropoff.id);
            dropoffs.insert(dropoff.id, dropoff);
        }
    }

    pub fn generate(input: &mut Input) -> Player {
        input.read_and_parse_line();
        let id = PlayerId(input.next_usize());
        let shipyard_x = input.next_i32();
        let shipyard_y = input.next_i32();

        let shipyard = Shipyard { owner: id, position: Position { x: shipyard_x, y: shipyard_y } };

        Player { id, shipyard, halite: 0, ship_ids: Vec::new(), dropoff_ids: Vec::new() }
    }
}
