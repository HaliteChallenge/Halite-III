package hlt

class Input(line: String?) {
    private val input: List<String> = line?.split(" ") ?: ArrayList()
    private var current: Int = 0

    val nextInt: Int
        get() = Integer.parseInt(input[current++])

    companion object {

        fun readInput(): Input {
            return Input(readLine())
        }

        fun readLine(): String? {
            return kotlin.io.readLine()
        }
    }
}
