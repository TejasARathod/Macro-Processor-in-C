
# Macro-Processor-in-C

Here we have designed a macro processor for a hypothetical microcontroller by creating relevant data structures required. When an assembly program is passed let’s say in our case with two macro definitions it shows generated assembly code.


## Table of Contents
- Features of this Macro processor
- Documentation
- Deployement
- Tech
- Authors
- License
## Features of this Macro Processor (Functions Created in it)
- DEFINE: This will first enter the macro name into the NAMTAB and then start entering the prototype in the DEFTAB. It will also store the start and end pointers in the NAMTAB.
- EXPAND: set a variable EXPANDING to true. We then get the lines from the macro definition and store the arguments in the ARGTAB.
- GETLINE: When EXPANDING is true, we get the next line of the macro from the DEFTAB and substitute the arguments from the ARGTAB. If EXPANDING is false, we just read the next line and write to the expanded file as it is.
- PROCESSLINE: We search the NAMTAB for the OPCODE. If the OPCODE exists, we call the EXPAND function, if the OPCODE is MACRO, we call the DEFINE function. Else, we write the source line to the expanded file as it is.

## Documentation

[Link for Detailed Documentation and Output Images](https://drive.google.com/drive/folders/1kdpRGq0X5V4jBPopB7N5zSoSBT_9F4Ht?usp=sharing)


## Deployment 
For this project a text file named input is require ; either the one uploaded can be used or ypu can make your own macro definitions just keeping in mind the format used in the given input text file.

To deploy this project run

```bash
  gcc main(4).c
```
Then
```bash
  main(4).c
```

## Tech Stack

**Language Used:** C

**IDE Used:** Codeblocks


## Authors

- [@tejasrathod](https://www.linkedin.com/in/tejas-rathod-923187189/)
- [@pravinsridhar](https://www.linkedin.com/in/pravin-sridhar-154058221/)



## License

[MIT](https://github.com/TejasARathod/Macro-Processor-in-C/blob/bba890f6404f1f29863a46d8d3df5c16e1468e6f/LICENSE)

