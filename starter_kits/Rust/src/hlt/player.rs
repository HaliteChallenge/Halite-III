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
    pub halite: i32,
    pub ships: HashMap<ShipId, Ship>,
    pub dropoffs: HashMap<DropoffId, Dropoff>,
}

impl Player {
    pub fn update(&mut self, input: &mut Input, max_halite: i32, num_ships: i32, num_dropoffs: i32, halite: i32) {
        self.halite = halite;

        self.ships.clear();
        for _ in 0..num_ships {
            let ship = Ship::generate(input, self.id, max_halite);
            self.ships.insert(ship.id, ship);
        }

        self.dropoffs.clear();
        for _ in 0..num_dropoffs {
            let dropoff = Dropoff::generate(input, self.id);
            self.dropoffs.insert(dropoff.id, dropoff);
        }
    }

    pub fn generate(input: &mut Input) -> Player {
        input.read_and_parse_line();
        let id = PlayerId(input.next_i32());
        let shipyard_x = input.next_i32();
        let shipyard_y = input.next_i32();

        let shipyard = Shipyard { owner: id, position: Position { x: shipyard_x, y: shipyard_y } };

        Player { id, shipyard, halite: 0, ships: HashMap::new(), dropoffs: HashMap::new() }
    }
}
