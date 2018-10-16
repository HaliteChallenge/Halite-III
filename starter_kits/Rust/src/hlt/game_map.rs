use hlt::entity::Entity;
use hlt::input::Input;
use hlt::map_cell::MapCell;
use hlt::map_cell::Structure;
use hlt::position::Position;
use std::cmp::min;

pub struct GameMap {
    pub width: usize,
    pub height: usize,
    pub cells: Vec<Vec<MapCell>>,
}

impl GameMap {
    pub fn at_position(&self, position: &Position) -> &MapCell {
        let normalized = self.normalize(position);
        &self.cells[normalized.y as usize][normalized.x as usize]
    }

    pub fn at_position_mut(&mut self, position: &Position) -> &mut MapCell {
        let normalized = self.normalize(position);
        &mut self.cells[normalized.y as usize][normalized.x as usize]
    }

    pub fn at_entity(&self, entity: &Entity) -> &MapCell {
        self.at_position(&entity.position())
    }

    pub fn at_entity_mut(&mut self, entity: &Entity) -> &mut MapCell {
        self.at_position_mut(&entity.position())
    }

    pub fn calculate_distance(&self, source: &Position, target: &Position) -> usize {
        let normalized_source = self.normalize(source);
        let normalized_target = self.normalize(target);

        let dx = (normalized_source.x - normalized_target.x).abs() as usize;
        let dy = (normalized_source.y - normalized_target.y).abs() as usize;

        let toroidal_dx = min(dx, self.width - dx);
        let toroidal_dy = min(dy, self.height - dy);

        return toroidal_dx + toroidal_dy;
    }

    pub fn normalize(&self, position: &Position) -> Position {
        let width = self.width as i32;
        let height = self.height as i32;
        let x = ((position.x % width) + width) % width;
        let y = ((position.y % height) + height) % height;
        Position { x, y }
    }

    pub fn update(&mut self, input: &mut Input) {
        input.read_and_parse_line();
        let update_count = input.next_usize();

        for _ in 0..update_count {
            input.read_and_parse_line();
            let x = input.next_usize();
            let y = input.next_usize();
            let halite = input.next_usize();

            self.cells[y][x].halite = halite;
        }
    }

    pub fn generate(input: &mut Input) -> GameMap {
        input.read_and_parse_line();
        let width = input.next_usize();
        let height = input.next_usize();

        let mut cells: Vec<Vec<MapCell>> = Vec::with_capacity(height);
        for y in 0..height {
            input.read_and_parse_line();

            let mut row: Vec<MapCell> = Vec::with_capacity(width);
            for x in 0..width {
                let halite = input.next_usize();

                let position = Position { x: x as i32, y: y as i32 };
                let cell = MapCell { position, halite, structure: Structure::None };
                row.push(cell);
            }

            cells.push(row);
        }

        GameMap { width, height, cells }
    }
}
