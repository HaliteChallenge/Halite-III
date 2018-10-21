<?php

namespace hlt;

class Dropoff extends Entity
{

    public function __construct(PlayerId $owner, EntityId $id, Position $position)
    {
        parent::__construct($owner, $id, $position);
    }

    static function _generate(PlayerId $playerId): Dropoff
    {
        $input = Input::readInput();
        $x = $input->getInt();
        $y = $input->getInt();

        /**
         * @var EntityId
         */
        $dropoffId = new EntityId($input->getInt());

        return new Dropoff($playerId, $dropoffId, new Position($x, $y));
    }
}
