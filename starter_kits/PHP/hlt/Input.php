<?php

namespace hlt;

use Exception;

class Input
{
    /**
     * @var String[]
     */
    public $input;
    /**
     * @var int
     */
    private $current = 0;

    public function __construct(string $line)
    {
        $this->input = explode(' ', $line);
    }

    public function getInt(): int
    {
        return (int)$this->input[$this->current++];
    }

    public static function readInput(): Input
    {
        return new Input(self::readLine());
    }

    public static function readLine(): string
    {
        try {
            $builder = '';

            for (; ($buffer = fread(STDIN, 1)) >= 0;) {
                if ($buffer === "\n") {
                    break;
                }
                if ($buffer === "\r") {
                    // Ignore carriage return if on windows for manual testing.
                    continue;
                }
                $builder .= $buffer;
            }

            return $builder;
        } catch (Exception $e) {
            Logger::log('Input connection from server closed. Exiting...');
            die();
        }
    }
}
