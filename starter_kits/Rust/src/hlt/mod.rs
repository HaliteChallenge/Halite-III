#[allow(dead_code)]
pub mod command;
#[allow(dead_code)]
pub mod constants;
#[allow(dead_code)]
pub mod direction;
#[allow(dead_code)]
pub mod dropoff;
#[allow(dead_code)]
pub mod entity;
#[allow(dead_code)]
pub mod game;
#[allow(dead_code)]
pub mod game_map;
#[allow(dead_code)]
pub mod log;
#[allow(dead_code)]
pub mod map_cell;
#[allow(dead_code)]
pub mod navi;
#[allow(dead_code)]
pub mod player;
#[allow(dead_code)]
pub mod position;
#[allow(dead_code)]
pub mod ship;
#[allow(dead_code)]
pub mod shipyard;

#[allow(dead_code)]
mod input;

#[derive(Copy, Clone, Eq, PartialEq, Hash)]
pub struct PlayerId(pub usize);

#[derive(Copy, Clone, Eq, PartialEq, Hash)]
pub struct DropoffId(pub usize);

#[derive(Copy, Clone, Eq, PartialEq, Hash)]
pub struct ShipId(pub usize);
