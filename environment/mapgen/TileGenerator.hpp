#ifndef TILEGENERATOR_H
#define TILEGENERATOR_H

#include "Generator.hpp"


namespace hlt {
    namespace mapgen {
        /**
         * Map generator using a "tiling" method. This generates a single tile of a map, and then duplicates
         * that tile for each player, arranging the tiles into an appropriate number of rows and columns.
         * The size of the tile is determined from the desired total hieght and width of the map,
         * and the number of players.
         */
        class TileGenerator : Generator {
        private:
            unsigned int width; /**< width (in cells) of the final map */
            unsigned int height; /**< height (in cells) of the final map */
            unsigned long tile_width; /**< width (in cells) of a single tile */
            unsigned long tile_height; /**< width (in cells) of a single tile */
            unsigned long num_tile_rows; /**< number of rows of tiles in map */
            unsigned long num_tile_cols; /**< number of cols of tiles in map */
            unsigned int num_players; /**< number of players who will be on the map */
            long blur_function(const long x_coord, const long y_coord, const hlt::Map &map) const;
        public:
            /** name is function to allow for possibly dynamically named subclasses */
            std::string name() const override;

            /** Generate a map given a list of players
             *
             * @param players: list of players of the game. Passed in for the purpose
             *      of assigning each player's factory to a fell
             * @return map with cells initialized (ie given production values, or
             *      assigned to alternate cell type)
             */
            hlt::Map generate(std::list<hlt::Player> &players)  override;


            /** TileGenerator class constructor
             *
             * @param parameters: Map parameters as defined in Generator.hpp
             * Note: dynamically calculates tile height and width based on  overall height and width and
             * number of players. Will cause an assertion error if given height and width cannot be divided into tiles
             * evenly (ie equally sized tiles arranged in a nice grid)
             */
            explicit TileGenerator(const MapParameters &parameters);

        };
    }
}

#endif // TILEGENERATOR_H
