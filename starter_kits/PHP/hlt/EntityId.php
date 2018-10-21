<?php

namespace hlt;

class EntityId {
    /**
     * @var int
     */
    public $id;

    public function __construct(int $id = -1)
    {
        $this->id = $id;
    }

    public function __toString()
    {
        return '' . $this->id;
    }

    public function equals($other): bool
    {
        if (!($other instanceof self)) {
            return false;
        }

        return $this->id === $other->id;
    }
}
