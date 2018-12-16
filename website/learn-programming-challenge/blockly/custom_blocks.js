const ACTION_COLOR = "#FFBE00";
const HELP_URL = 'https://forums.halite.io/t/halite-blockly-create-a-halite-bot-with-blocks/1132';
Blockly.Msg['TEXT_PRINT_TOOLTIP'] = 'Logging is how your bot communicates with you. Read logs in the panel under the visualizer - scroll all the way down to see your logging statements.';

Blockly.BlockSvg.START_HAT = true;

Blockly.Blocks['shipyard'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("my shipyard");
        this.appendStatementInput("behavior")
            .setCheck(null)
            .appendField("does");
        this.appendDummyInput()
            .appendField("every turn");
        this.setColour(120);
        this.setTooltip("Your shipyard will run this code every turn.");
        this.setHelpUrl(HELP_URL);
        this.setDeletable(false);
    }
};

Blockly.Blocks['my_halite'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("my current halite");
        this.setOutput(true, "Number");
        this.setColour(230);
        this.setTooltip("Your goal is to collect halite. Direct your ships to bring their halite cargo back to your shipyard to increase your current halite.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['spawn_ship'] = {
    init: function() {
        this.actionType = 'shipyard';
        this.appendDummyInput()
            .appendField("spawn a new ship (1000 halite)");
        this.setPreviousStatement(true, null);
        this.setColour(ACTION_COLOR);
        this.setTooltip("The shipyard builds a new ship for 1,000 halite. If there is already a ship on the shipyard, your new ship will collide with it, destroying both ships.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['nothing_on_top'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("nothing on top of the shipyard");
        this.setOutput(true, "Boolean");
        this.setColour(210);
        this.setTooltip("");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['move_towards'] = {
    init: function() {
        this.actionType = 'each_ship';
        this.appendValueInput("NAME")
            .setCheck("Position")
            .appendField("move this ship toward");
        this.setPreviousStatement(true, null);
        this.setColour(ACTION_COLOR);
        this.setTooltip("Ships can move one square in a cardinal direction per turn toward a given target. Each move costs 10% of the halite available in the sea at the ship’s starting location, debited from the ships’ cargo.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['move_unsafe'] = {
    init: function() {
        this.actionType = 'each_ship';
        this.appendValueInput("NAME")
            .setCheck("Direction")
            .appendField("move this ship");
        this.setPreviousStatement(true, null);
        this.setColour(ACTION_COLOR);
        this.setTooltip("Ships can move one square in a cardinal direction per turn. Each move costs 10% of the halite available in the sea at the ship’s starting location, debited from the ships’ cargo.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['nearest_shipyard_or_dropoff'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("the nearest shipyard or dropoff");
        this.setOutput(true, "Position");
        this.setColour(290);
        this.setTooltip("The position of the closest shipyard or dropoff.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['each_ship'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("my ships");
        this.appendStatementInput("commands")
            .setCheck(null)
            .appendField("do");
        this.appendDummyInput()
            .appendField("on every turn");
        this.setColour(120);
        this.setTooltip("");
        this.setHelpUrl(HELP_URL);
        console.log(this);
        this.setDeletable(false);
    }
};

Blockly.Blocks['this_ship_halite'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("this ship's cargo");
        this.setOutput(true, "Number");
        this.setColour(230);
        this.setTooltip("Returns how much halite this ship is carrying. Ships have a capacity of 1,000 halite.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['nearest_enemy_ship'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("the nearest enemy ship");
        this.setOutput(true, "Position");
        this.setColour(290);
        this.setTooltip("");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['distance_to'] = {
    init: function() {
        this.appendValueInput("distance to")
            .setCheck("Position")
            .appendField("the distance to");
        this.setOutput(true, "Number");
        this.setColour(230);
        this.setTooltip("The Manhattan distance between two locations, accounting for the wraparound of the game map.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['nearest_patch_of_halite'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("a nearby patch of halite");
        this.setOutput(true, "Position");
        this.setColour(290);
        this.setTooltip("The position of a nearby rich location of halite.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['halite_under_me'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("the halite under this ship");
        this.setOutput(true, "Number");
        this.setColour(230);
        this.setTooltip("Returns the halite concentration in the sea beneath the ship’s current location.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['ship_cost'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("the price of a ship (1000)");
        this.setOutput(true, "Number");
        this.setColour(230);
        this.setTooltip("The price of building a new ship at your shipyard is 1,000 halite.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['dropoff_cost'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("the price of a dropoff (4000)");
        this.setOutput(true, "Number");
        this.setColour(230);
        this.setTooltip("The price of converting a ship to a new dropoff is 4,000 halite, deducted from total current stored halite. The converted ship’s halite cargo and the halite in the sea under the new dropoff is credited to the player. These credits resolve first, and can be used toward the cost of the dropoff.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['map_width'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("the width of the map");
        this.setOutput(true, "Number");
        this.setColour(230);
        this.setTooltip("The width of the map you are playing on.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['map_height'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("the height of the map");
        this.setOutput(true, "Number");
        this.setColour(230);
        this.setTooltip("The height of the map you are playing on.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['do_nothing'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("do nothing and collect halite");
        this.setPreviousStatement(true, null);
        this.setColour(ACTION_COLOR);
        this.setTooltip("Each turn, ships that stay still can collect 25% of the halite from the sea at their present location, rounded up to the nearest integer.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['construct_dropoff'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("convert into a dropoff (4000 halite)");
        this.setPreviousStatement(true, null);
        this.setColour(ACTION_COLOR);
        this.setTooltip("Ships can be converted into dropoff sites at their present location. The conversion costs 4,000 halite, deducted from total current stored halite. Ships can store halite at a dropoff point just as they would at the shipyard.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['nearby_enemy'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("there is an enemy ship");
        this.setOutput(true, "Boolean");
        this.setColour(210);
        this.setTooltip("");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['at_dropoff'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("this ship is at home");
        this.setOutput(true, "Boolean");
        this.setColour(210);
        this.setTooltip("Checks if the ship is currently at a shipyard.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['direction'] = {
    init: function() {
        this.appendDummyInput()
            .appendField(new Blockly.FieldDropdown([["north","NORTH"], ["south","SOUTH"], ["east","EAST"], ["west","WEST"]]), "DIRECTION");
        this.setOutput(true, "Direction");
        this.setColour(30);
        this.setTooltip("");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['ship_action'] = {
    init() {
        this.actionType = 'each_ship';
        this.appendDummyInput()
            .appendField(new Blockly.FieldDropdown([
                ["north","NORTH"],
                ["south","SOUTH"],
                ["east","EAST"],
                ["west","WEST"],
            ]), "ACTION");
    }
};

Blockly.Blocks['random_direction'] = {
    init: function() {
        this.appendDummyInput()
            .appendField("a random direction");
        this.setOutput(true, "Direction");
        this.setColour(30);
        this.setTooltip("Returns a random cardinal direction.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['set_ship_state'] = {
    init: function() {
        this.actionType = 'each_ship';
        this.appendValueInput("STATUS")
            .setCheck("String")
            .appendField("set this ship's status to");
        this.setPreviousStatement(true, null);
        this.setNextStatement(true, null);
        this.setColour(0);
        this.setTooltip("Set a ship’s status to control what it does from turn to turn. This makes your ship stateful so that it can save knowledge and complete tasks over multiple turns.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.Blocks['ship_state'] = {
    init: function() {
        this.actionType = 'each_ship';
        this.appendDummyInput()
            .appendField("this ship's status");
        this.setOutput(true, "String");
        this.setColour(0);
        this.setTooltip("Returns your ship’s current status.");
        this.setHelpUrl(HELP_URL);
    }
};

Blockly.JavaScript['shipyard'] = function(block) {
    const statements_behavior = Blockly.JavaScript.statementToCode(block, 'behavior');
    return statements_behavior;
};

Blockly.JavaScript['my_halite'] = function(block) {
    const code = 'me.haliteAmount';
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['spawn_ship'] = function(block) {
    // TODO: decrement available halite
    // TODO: log error if user spends too much halite anyways
    return 'commandQueue.push(me.shipyard.spawn());';
};

Blockly.JavaScript['nothing_on_top'] = function(block) {
    const code = '!gameMap.get(me.shipyard).isOccupied';
    // TODO: Change ORDER_NONE to the correct strength.
    return [code, Blockly.JavaScript.ORDER_LOGICAL_NOT];
};

Blockly.JavaScript['move_towards'] = function(block) {
    const value_name = Blockly.JavaScript.valueToCode(block, 'NAME', Blockly.JavaScript.ORDER_ATOMIC);
    const code = `
commandQueue.push(currentShip.move(gameMap.naiveNavigate(currentShip, ${value_name})));
`;
    return code;
};

Blockly.JavaScript['move_unsafe'] = function(block) {
    const value_name = Blockly.JavaScript.valueToCode(block, 'NAME', Blockly.JavaScript.ORDER_ATOMIC);
    const code = `commandQueue.push(currentShip.move(gameMap.naiveNavigate(currentShip, currentShip.position.directionalOffset(${value_name}))));`;
    return code;
};

Blockly.JavaScript['nearest_shipyard_or_dropoff'] = function(block) {
    const code = 'nearestShipyard(gameMap, me, currentShip)';
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['each_ship'] = function(block) {
    const statements_commands = Blockly.JavaScript.statementToCode(block, 'commands');
    const code = `
for (const currentShip of me.getShips()) {
    ${statements_commands}
}
`;
    return code;
};

Blockly.JavaScript['this_ship_halite'] = function(block) {
    const code = 'currentShip.haliteAmount';
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['nearest_enemy_ship'] = function(block) {
    const code = 'nearestEnemy(game, currentShip).position';
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['distance_to'] = function(block) {
    const value_distance_to = Blockly.JavaScript.valueToCode(block, 'distance to', Blockly.JavaScript.ORDER_ATOMIC);
    const code = `gameMap.calculateDistance(currentShip.position, ${value_distance_to})`;
    return [code, Blockly.JavaScript.ORDER_FUNCTION_CALL];
};

Blockly.JavaScript['nearest_patch_of_halite'] = function(block) {
    const code = 'nearestHalite(gameMap, me, currentShip)';
    return [code, Blockly.JavaScript.ORDER_FUNCTION_CALL];
};

Blockly.JavaScript['halite_under_me'] = function(block) {
    var code = 'gameMap.get(currentShip.position).haliteAmount';
    return [code, Blockly.JavaScript.ORDER_FUNCTION_CALL];
};

Blockly.JavaScript['ship_cost'] = function(block) {
    var code = '1000';
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['dropoff_cost'] = function(block) {
    var code = '4000';
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['map_width'] = function(block) {
    var code = 'gameMap.width';
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['map_height'] = function(block) {
    var code = 'gameMap.height';
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['do_nothing'] = function(block) {
    return 'commandQueue.push(currentShip.stayStill());';
};

Blockly.JavaScript['construct_dropoff'] = function(block) {
    return 'commandQueue.push(currentShip.makeDropoff());';
};

Blockly.JavaScript['text_print'] = function(block) {
    const text = Blockly.JavaScript.valueToCode(block, 'TEXT', Blockly.JavaScript.ORDER_ATOMIC);
    return `logging.info(${text});`;
};

Blockly.JavaScript['nearby_enemy'] = function(block) {
    const code = 'existsEnemy(game)';
    return [code, Blockly.JavaScript.ORDER_FUNCTION_CALL];
};

Blockly.JavaScript['at_dropoff'] = function(block) {
    const code = 'atDropoff(gameMap, me, currentShip)';
    return [code, Blockly.JavaScript.ORDER_FUNCTION_CALL];
};

Blockly.JavaScript['direction'] = function(block) {
    const dropdown_direction = block.getFieldValue('DIRECTION').toLowerCase();
    const code = `Direction.${dropdown_direction[0].toUpperCase() + dropdown_direction.slice(1)}`;
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};


Blockly.JavaScript['random_direction'] = function(block) {
    const code = `Direction.getAllCardinals()[Math.floor(Math.random() * 4)]`;
    return [code, Blockly.JavaScript.ORDER_ATOMIC];
};

// Copied from Blockly source; the provideFunction_ stuff isn't working.
Blockly.JavaScript['math_random_int'] = function(block) {
  // Random integer between [X] and [Y].
  var argument0 = Blockly.JavaScript.valueToCode(block, 'FROM',
      Blockly.JavaScript.ORDER_COMMA) || '0';
  var argument1 = Blockly.JavaScript.valueToCode(block, 'TO',
      Blockly.JavaScript.ORDER_COMMA) || '0';
  var code = 'mathRandomInt(' + argument0 + ', ' + argument1 + ')';
  return [code, Blockly.JavaScript.ORDER_FUNCTION_CALL];
};

Blockly.JavaScript['ship_state'] = function(block) {
    const code = `shipState.get(currentShip.id)`;
    return [code, Blockly.JavaScript.ORDER_FUNCTION_CALL];
};

Blockly.JavaScript['set_ship_state'] = function(block) {
    const arg = Blockly.JavaScript.valueToCode(block, 'STATUS', Blockly.JavaScript.ORDER_COMMA) || 'null';
    const code = `shipState.set(currentShip.id, ${arg});`;
    return code;
};
