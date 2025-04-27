; ModuleID = 'main'
source_filename = "main"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @hello(i32 %0, i32 %1) {
entry:
  %value_b = alloca i32, align 4
  %value_a = alloca i32, align 4
  store i32 %0, i32* %value_a, align 4
  store i32 %1, i32* %value_b, align 4
  %value_a1 = load i32, i32* %value_a, align 4
  %cmptmp = icmp sge i32 %value_a1, 5
  br i1 %cmptmp, label %then, label %else

then:                                             ; preds = %entry
  %value_a2 = load i32, i32* %value_a, align 4
  %multmp = mul i32 %value_a2, 2
  store i32 %multmp, i32* %value_a, align 4
  br label %merge

else:                                             ; preds = %entry
  store i32 11, i32* %value_a, align 4
  br label %merge

merge:                                            ; preds = %else, %then
  %value_a3 = load i32, i32* %value_a, align 4
  ret i32 %value_a3
}

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 3237, i32* %x, align 4
  %x1 = load i32, i32* %x, align 4
  %cmptmp = icmp eq i32 %x1, 3237
  br i1 %cmptmp, label %then, label %else

then:                                             ; preds = %entry
  %x2 = load i32, i32* %x, align 4
  %x3 = load i32, i32* %x, align 4
  %0 = call i32 @hello(i32 %x2, i32 %x3)
  store i32 %0, i32* %x, align 4
  %x4 = load i32, i32* %x, align 4
  %printfCall = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %x4)
  br label %merge

else:                                             ; preds = %entry
  store i32 42, i32* %x, align 4
  br label %merge

merge:                                            ; preds = %else, %then
  ret i32 0
}

declare i32 @printf(i8*, ...)
