use hlt::command::Command;
use hlt::constants::Constants;
use hlt::game_map::GameMap;
use hlt::input::Input;
use hlt::log::Log;
use hlt::player::Player;
use hlt::PlayerId;
use std::cell::RefCell;
use std::ops::DerefMut;
use std::rc::Rc;
use hlt::map_cell::Structure;

pub struct Game {
    pub log: Rc<RefCell<Log>>,
    pub constants: Constants,
    pub turn_number: i32,
    pub my_id: PlayerId,
    pub players: Vec<Player>,
    pub my_index: usize,
    pub game_map: GameMap,
    input: Input,
}

impl Game {
    pub fn new() -> Game {
        let log = Rc::new(RefCell::new(Log::new()));
        let mut input = Input::new(&log);
        let constants = Constants::new(log.borrow_mut().deref_mut(), &input.read_and_return_line());

        input.read_and_parse_line();
        let num_players = input.next_i32();
        let my_id = PlayerId(input.next_i32());

        log.borrow_mut().open(my_id.0);

        let mut players: Vec<Player> = Vec::new();
        for _ in 0..num_players {
            players.push(Player::generate(&mut input));
        }

        let game_map = GameMap::generate(&mut input);

        Game { log, constants, turn_number: 0, my_id, players, my_index: my_id.0 as usize, game_map, input }
    }

    pub fn ready(name: &str) {
        println!("{}", name);
    }

    pub fn update_frame(&mut self) {
        let input = &mut self.input;

        input.read_and_parse_line();
        self.turn_number = input.next_i32();

        self.log.borrow_mut().log(&format!("=============== TURN {} ================", self.turn_number));

        for _ in 0..self.players.len() {
            input.read_and_parse_line();
            let current_player_id = input.next_i32();
            let num_ships = input.next_i32();
            let num_dropoffs = input.next_i32();
            let halite = input.next_i32();

            self.players[current_player_id as usize].update(input, self.constants.max_halite, num_ships, num_dropoffs, halite);
        }

        self.game_map.update(input);

        for player in &self.players {
            for (id, ship) in &player.ships {
                self.game_map.at_entity_mut(ship).mark_unsafe(*id);
            }

            let shipyard = &player.shipyard;
            self.game_map.at_entity_mut(shipyard).structure = Structure::Shipyard(player.id);

            for (id, dropoff) in &player.dropoffs {
                self.game_map.at_entity_mut(dropoff).structure = Structure::Dropoff(*id);
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
