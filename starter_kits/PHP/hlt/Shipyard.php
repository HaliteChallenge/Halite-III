<?php

namespace hlt;

class Shipyard extends Entity {
    public function __construct( PlayerId $owner,  Position $position)
    {
        parent::__construct($owner, new EntityId(), $position);
    }

    /**
     * @return Command
     */
    public function spawn() {
        return Command::spawnShip();
    }
}
