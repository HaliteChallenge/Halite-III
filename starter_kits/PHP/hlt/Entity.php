<?php

namespace hlt;

class Entity {
    /**
     * @var PlayerId
     */
    public $owner;

    /**
     * @var EntityId
     */
    public $id;

    /**
     * @var Position
     */
    public $position;

    public function __construct( PlayerId $owner,  EntityId $id,  Position $position)
    {
        $this->owner = $owner;
        $this->id = $id;
        $this->position = $position;
    }

    public function equals($other): bool
    {
        if (!($other instanceof self)) {
            return false;
        }

        return $this->owner->equals($other->owner) && $this->id->equals($other->id) && $this->position->equals($other->position);
    }
}
