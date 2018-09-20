package hlt

import scala.io.StdIn

object Input {
  def readInput = new Input(readLine)

  def readLine: String = try {
    StdIn.readLine()
  } catch {
    case e: Exception =>
      Log.log("Input connection from server closed. Exiting...")
      System.exit(0)
      throw new IllegalStateException(e)
  }
}

class Input(val line: String) {
  var input: Array[String] = line.split(" ")
  var current: Int = 0

  def getInt: Int = {
    val nextInt = input(current).toInt
    current = current + 1
    nextInt
  }
}
