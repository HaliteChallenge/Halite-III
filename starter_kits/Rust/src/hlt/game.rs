use hlt::command::Command;
use hlt::constants::Constants;
use hlt::game_map::GameMap;
use hlt::input::Input;
use hlt::log::Log;
use hlt::player::Player;
use hlt::PlayerId;
use std::cell::RefCell;
use std::rc::Rc;
use std::ops::DerefMut;

pub struct Game {
    pub log: Rc<RefCell<Log>>,
    pub constants: Rc<Constants>,
    pub turn_number: i32,
    pub my_id: PlayerId,
    pub players: Vec<Rc<RefCell<Player>>>,
    pub me: Rc<RefCell<Player>>,
    pub game_map: Rc<RefCell<GameMap>>,
    input: Input,
}

impl Game {
    pub fn new() -> Game {
        let log = Rc::new(RefCell::new(Log::new()));
        let mut input = Input::new(&log);
        let constants = Constants::new(log.borrow_mut().deref_mut(), &input.read_and_return_line());
        let constants = Rc::new(constants);

        input.read_and_parse_line();
        let num_players = input.next_i32();
        let my_id = PlayerId(input.next_i32());

        log.borrow_mut().open(my_id.0);

        let mut players: Vec<Rc<RefCell<Player>>> = Vec::new();
        for _ in 0..num_players {
            let player = Player::generate(&mut input);
            players.push(Rc::new(RefCell::new(player)));
        }
        let me = players[my_id.0 as usize].clone();

        let game_map = GameMap::generate(&mut input);
        let game_map = Rc::new(RefCell::new(game_map));

        Game { log, constants, turn_number: 0, my_id, players, me, game_map, input }
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

            self.players[current_player_id as usize].borrow_mut().update(input, self.constants.max_halite, num_ships, num_dropoffs, halite);
        }

        self.game_map.borrow_mut().update(input);

        for player in &self.players {
            for (_, ship) in &player.borrow().ships {
                let cell = self.game_map.borrow_mut().at_entity(&**ship);
                cell.borrow_mut().mark_unsafe(ship);
            }

            let shipyard = &player.borrow().shipyard;
            let cell = self.game_map.borrow_mut().at_entity(&**shipyard);
            cell.borrow_mut().structure = Some(shipyard.clone());

            for (_, dropoff) in &player.borrow().dropoffs {
                let cell = self.game_map.borrow_mut().at_entity(&**dropoff);
                cell.borrow_mut().structure = Some(dropoff.clone());
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
