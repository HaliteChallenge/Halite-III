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
use std::cell::RefCell;
use std::collections::HashMap;
use std::ops::DerefMut;
use std::rc::Rc;

pub struct Game {
    pub log: Rc<RefCell<Log>>,
    pub constants: Constants,
    pub turn_number: usize,
    pub my_id: PlayerId,
    pub players: Vec<Player>,
    pub ships: HashMap<ShipId, Ship>,
    pub dropoffs: HashMap<DropoffId, Dropoff>,
    pub game_map: GameMap,
    input: Input,
}

impl Game {
    pub fn new() -> Game {
        let log = Rc::new(RefCell::new(Log::new()));
        let mut input = Input::new(&log);
        let constants = Constants::new(log.borrow_mut().deref_mut(), &input.read_and_return_line());

        input.read_and_parse_line();
        let num_players = input.next_usize();
        let my_id = PlayerId(input.next_usize());

        log.borrow_mut().open(my_id.0);

        let mut players: Vec<Player> = Vec::new();
        for _ in 0..num_players {
            players.push(Player::generate(&mut input));
        }

        let game_map = GameMap::generate(&mut input);

        Game {
            log,
            constants,
            turn_number: 0,
            my_id,
            players,
            ships: HashMap::new(),
            dropoffs: HashMap::new(),
            game_map,
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

        self.log.borrow_mut().log(&format!("=============== TURN {} ================", self.turn_number));

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

        self.game_map.update(input);

        for player in &self.players {
            for ship_id in &player.ship_ids {
                let ship = &self.ships[ship_id];
                self.game_map.at_entity_mut(ship).mark_unsafe(*ship_id);
            }

            let shipyard = &player.shipyard;
            self.game_map.at_entity_mut(shipyard).structure = Structure::Shipyard(player.id);

            for dropoff_id in &player.dropoff_ids {
                let dropoff = &self.dropoffs[dropoff_id];
                self.game_map.at_entity_mut(dropoff).structure = Structure::Dropoff(*dropoff_id);
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
