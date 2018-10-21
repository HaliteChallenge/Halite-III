<?php

namespace hlt;

class PlayerId {
    /**
     * @var int
     */
    public $id;

    public function __construct(int $id)
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
