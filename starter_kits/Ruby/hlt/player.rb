require_relative 'hlt'
include InputReader

# Player object containing all items/metadata pertinent to the other players.
class Player
  attr_accessor :id, :hipyard, :halite_amount, :shipyard, :ships, :dropoffs
  def initialize(player_id, shipyard, halite=0)
    @id = player_id
    @shipyard = shipyard
    @halite_amount = halite
    @ships = {}
    @dropoffs = {}
  end

  # Returns a singular ship mapped by the ship id
  # @param ship_id: The ship id of the ship you wish to return
  # @return [Ship] The ship object
  def get_ship(ship_id)
    @ships[ship_id]
  end

  # Returns all ship objects in an array
  def get_ships
    @ships.values
  end

  # Returns a singular dropoff mapped by its id
  # @oaram dropoff_id: The dropoff id to return
  # @return [Dropoff] The dropoff object
  def get_dropoff(dropoff_id)
    @dropoffs[dropoff_id]
  end

    # Returns all dropoff objects in an array
    def get_dropoffs
      @dropoffs.values
    end

    # Check whether the player has a ship with a given ID.
    #
    # Useful if you track ships via IDs elsewhere and want to make
    # sure the ship still exists.
    # @param ship_id: The ID to check.
    # @return [Bool] True if and only if the ship exists.
    def has_ship(ship_id)
      @ships.key?(ship_id)
    end

    # Creates a player object from the input given by the game engine
    # @return [Player] The player object
    def self.generate
      player, shipyard_x, shipyard_y = read_input.split.map(&:to_i)
      position = Positionals::Position.new(shipyard_x, shipyard_y)
      shipyard = Shipyard.new(player, -1, position)
      new(player, shipyard)
    end

    # Updates this player object considering the input from the game engine
    # for the current specific turn.
    # @param num_ships: The number of ships this player has this turn
    # @param num_dropoffs: The number of dropoffs this player has this turn
    # @param halite: How much halite the player has in total
    # @return: [Self]
    def update(num_ships, num_dropoffs, halite)
      @halite_amount = halite
      @ships = generate_ships(num_ships)
      @dropoffs = generate_dropoffs(num_dropoffs)
    end

    # Generates a hash of Ship objects.
    # @param num_ships: The number of ships to generate
    # @return: [{ ship_id: Ship }]
    def generate_ships(num_ships)
      ship_array = []
      num_ships.times { ship_array << Ship.generate(@id) }
      Hash[ship_array]
    end

    # Generates a hash of Dropoff objects.
    # @param num_ships: The number of dropoffs to generate
    # @return: [{ dropoff_id: Ship }]
    def generate_dropoffs(num_dropoffs)
      dropoff_array = []
      num_dropoffs.times { dropoff_array << Dropoff.generate(@id) }
      Hash[dropoff_array]
    end
end
