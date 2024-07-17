with Ada.Text_IO;
with Ada.Integer_Text_IO;

procedure Out_Mode is
   procedure DiffModes (
      A : Integer;
      B : out Integer;
      C : in out Integer
   ) is
   begin
      Ada.Integer_Text_IO.Put(Item => A); Ada.Text_IO.New_Line;
      Ada.Integer_Text_IO.Put(Item => B); Ada.Text_IO.New_Line;
      Ada.Integer_Text_IO.Put(Item => C); Ada.Text_IO.New_Line;

      B := 100;
      C := C * 2;
   end DiffModes;

   X : Integer := 12;
   Y : Integer := 44;
   Z : Integer := 30;
begin
   DiffModes(X, Y, Z);
   Ada.Text_IO.Put_Line("--------------------------------------------------");
   Ada.Integer_Text_IO.Put(Item => X); Ada.Text_IO.New_Line;
   Ada.Integer_Text_IO.Put(Item => Y); Ada.Text_IO.New_Line;
   Ada.Integer_Text_IO.Put(Item => Z); Ada.Text_IO.New_Line;
end Out_Mode;

