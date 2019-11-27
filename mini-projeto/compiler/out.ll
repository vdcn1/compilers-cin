read 672 bytes from file 'test.c' and it has 672
@nom_nom = dso_local local_unnamed_addr global i32 88, allign 4
@arroz = dso_local local_unnamed_addr global i32 0, allign 4
define dso_local int @aqua_baldo() local_unnamed_addr #0 {
	 %1 = mul i32 32, 5
	 %2 = srem i32 %0, 11
	 %3 = sub i32 %2, 2
	 %4 = add nsw i32 6, %0
	 %5 = srem i32 11, %4
	 %6 = sub i32 1, %1
	 %7 = srem i32 %3, %0
	 %8 = add nsw i32 %6, %7
	 ret i32 %8
}
define dso_local int @function7(i32, i32) local_unnamed_addr #0 {
	 %3 = srem i32 %0, %1
	 %4 = sub i32 0, %4
	 %5 = add nsw i32 %2, %0
	 %6 = mul i32 %4, %5
	 %7 = sdiv i32 %6, 2
	 %8 = sub i32 %4, %7
	 ret i32 %8
}
define dso_local void @kk(i32) local_unnamed_addr #0 {
	 %2 = sdiv i32 %0, 2
	 ret void
}
define dso_local int @aster(i32, i32, i32) local_unnamed_addr #0 {
	 %4 = mul i32 3, %0
	 %5 = mul i32 5, %1
	 %6 = add nsw i32 %4, %5
	 %7 = mul i32 2, %2
	 %8 = add nsw i32 %6, %7
	 %9 = sdiv i32 %8, 10
	 %10 = mul i32 255, %3
	 %11 = sdiv i32 %10, %0
	 %12 = mul i32 255, %4
	 %13 = sdiv i32 %12, %10
	 %14 = sub i32 %6, 127
	 %15 = mul i32 255, %14
	 %16 = sdiv i32 %15, %12
	 %17 = add nsw i32 %11, %13
	 %18 = add nsw i32 %17, %16
	 ret i32 %18
}
define dso_local int @main() local_unnamed_addr #0 {
	 %1 = sub i32 %0, %2
	 ret i32 %1
}
