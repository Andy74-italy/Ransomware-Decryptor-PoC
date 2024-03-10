# Ransomware-Decryptor-PoC
PoC decryptor for Rhysida ransomware - recover files encrypted by Rhysida with this proof-of-concept implementation. Not an official decryptor, provided solely for demonstration purposes to showcase the possibility of restoring files affected by the ransomware.

# Description
This repository contains a Proof of Concept (PoC) for decrypting files affected by the **Rhysida ransomware**. It is important to note that this PoC is NOT complete and will NOT be further developed. The code within serves a purely demonstrative purpose, showcasing how it was possible to restore files encrypted by the **Rhysida ransomware**, demonstrating the feasibility of file recovery.

Associated with this repository are two articles that provide in-depth insights into the analysis of the malware and the vulnerability that facilitated the creation of the decryptor. These articles can be accessed at the following links:

[Rhysida ransomware Malware Analysis - Part 1: Dynamic analysis](https://www.secjuice.com/rhysida-ransomware-malware-analysis-part-1-dynamic-analysis): Malware analysis to understand the workings of the ransomware.

[Rhysida ransomware Malware Analysis - Part2: How to decrypt](https://www.secjuice.com/rhysida-ransomware-malware-analysis-part2-how-to-decrypt): Description of the vulnerability that allowed the creation of the decryptor, along with the steps taken to develop the code.

Utilize the provided code as a reference for understanding the process of decrypting files affected by the Rhysida ransomware.

# Usage
To integrate this Proof of Concept (PoC) into your project, follow the steps below:

Download the repository and add it to the original **libtomcrypt** project's solution available at [libtom/libtomcrypt](https://github.com/libtom/libtomcrypt).

Ensure that the libtommath library is compiled and included in the solution. The **libtommath** library can be obtained from [libtom/libtommath](https://github.com/libtom/libtommath).

Incorporate the necessary dependencies and ensure that the project builds successfully.

# Disclaimer
This tool is provided as-is, and its usage is at the user's own risk. The developers and contributors disclaim any responsibility for potential damages caused by the use of this tool.
