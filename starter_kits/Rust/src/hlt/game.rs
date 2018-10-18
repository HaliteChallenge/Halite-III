use hlt::command::Command;
use hlt::constants::Constants;
use hlt::dropoff::Dropoff;
use hlt::DropoffId;
use hlt::game_map::GameMap;
use hlt::input::Input;
use hlt::log::Log;
use hlt::map_cell::Structure;
use hlt::player::Player;
use hlt::PlayerId;
use hlt::ship::Ship;
use hlt::ShipId;
use std::collections::HashMap;

pub struct Game {
    pub constants: Constants,
    pub turn_number: usize,
    pub my_id: PlayerId,
    pub players: Vec<Player>,
    pub ships: HashMap<ShipId, Ship>,
    pub dropoffs: HashMap<DropoffId, Dropoff>,
    pub map: GameMap,
    input: Input,
}

impl Game {
    pub fn new() -> Game {
        let mut input = Input::new();
        let constants = Constants::new(&input.read_and_return_line());

        input.read_and_parse_line();
        let num_players = input.next_usize();
        let my_id = PlayerId(input.next_usize());

        Log::open(my_id.0);

        let mut players: Vec<Player> = Vec::new();
        for _ in 0..num_players {
            players.push(Player::generate(&mut input));
        }

        let map = GameMap::generate(&mut input);

        Game {
            constants,
            turn_number: 0,
            my_id,
            players,
            ships: HashMap::new(),
            dropoffs: HashMap::new(),
            map,
            input
        }
    }

    pub fn ready(name: &str) {
        println!("{}", name);
    }

    pub fn update_frame(&mut self) {
        let input = &mut self.input;

        input.read_and_parse_line();
        self.turn_number = input.next_usize();

        Log::log(&format!("=============== TURN {} ================", self.turn_number));

        self.ships.clear();
        self.dropoffs.clear();

        for _ in 0..self.players.len() {
            input.read_and_parse_line();
            let current_player_id = input.next_usize();
            let num_ships = input.next_usize();
            let num_dropoffs = input.next_usize();
            let halite = input.next_usize();

            self.players[current_player_id].update(
                input,
                self.constants.max_halite,
                &mut self.ships,
                &mut self.dropoffs,
                num_ships,
                num_dropoffs,
                halite);
        }

        self.map.update(input);

        for player in &self.players {
            let shipyard = &player.shipyard;
            self.map.at_entity_mut(shipyard).structure = Structure::Shipyard(player.id);

            for dropoff_id in &player.dropoff_ids {
                let dropoff = &self.dropoffs[dropoff_id];
                self.map.at_entity_mut(dropoff).structure = Structure::Dropoff(*dropoff_id);
            }
        }
    }

    pub fn end_turn(commands: &[Command]) {
        for command in commands {
            print!("{} ", command.0);
        }
        println!();
    }
}
