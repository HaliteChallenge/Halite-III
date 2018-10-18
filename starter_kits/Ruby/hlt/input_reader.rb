module InputReader
  # Reads input from stdin, exiting if EOFError occurs
  def read_input
    begin
      readline
    rescue EOFError => eof
      raise SystemExit(eof)
    end
  end
end
